#include <sys/stat.h>
#include <errno.h>
#include <time.h>

#include "gamegui/gamegui.h"

#include "waveformwidget.h"
#include "vumeter.h"

#include "audio.h"
#include "fileio.h"

#ifdef GCW0

#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 200
#define FULLSCREEN    true

#else  // NOT GCW0

#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 200
#define FULLSCREEN    false

#endif // GCW0

enum
{
    ERR_NO_AUDIO = 1,
    ERR_NO_MIXER,
    ERR_INIT_SDL,
    ERR_NO_MEMORY
}
app_error_codes;

/* Define a few globals */
#define AUDIO_DIR "/audio"

#define OUTPUT_FILENAME_SIZE 50
static char output_filename[OUTPUT_FILENAME_SIZE];

/* AUDIO RELATED variables */
static PCM_Play*    play    = NULL;
static PCM_Capture* capture = NULL;

// Default recording settings
static unsigned int sample_rate = 44100;
static int          bitdepth    = 16;
static Mixer*       mixer_mic   = NULL;
//static Mixer*       mixer_pcm   = NULL;
#define VOLUME_STEP 0.1

// Count of frames to record at each call
// for now equal to the sample freq
#define FRAME_COUNT  44100
#define CHANNELS     1
#define INITIAL_SECS 10
// buffer for 5 seconds initially
#define AUDIO_BUF_INITIAL_SIZE ( CHANNELS * FRAME_COUNT *  INITIAL_SECS)
static u_int32_t     audio_buf_size = AUDIO_BUF_INITIAL_SIZE;
//static const u_int32_t AUDIO_BUF_INC_SIZE = FRAME_COUNT * CHANNELS * INITIAL_SECS;
static int16_t*      audio_buf1       = NULL;
static int16_t*      audio_buf2       = NULL;
static u_int32_t     audio_buf_index1 = 0;
static u_int32_t     audio_buf_index2 = 0;
static u_int32_t     audio_buf_count  = 0;

static float         current_volume = 0.8;
typedef enum  { STOPPED, PLAYING, RECORDING } audio_state_t;
static audio_state_t audio_state = STOPPED;

/* UI Related variables */
static GGScreen*      screen     = NULL;
static GGWaveform*    wfw        = NULL;
static GGLabel*       lbl_title  = NULL;
static GGImageButton* btn_exit   = NULL;
static GGLabel*       lbl_file   = NULL;
static GGButton*      btn_new    = NULL;
static GGImageButton* btn_record = NULL;
static GGImageButton* btn_play   = NULL;
static GGImageButton* btn_stop   = NULL;
static GGImageButton* btn_replay = NULL;
static GGImageButton* btn_mic    = NULL;
static GGVUMeter*     vumeter    = NULL;
static GGHelpBar*     helpbar1   = NULL;

static SDL_Color      dark_gray = {0x2f, 0x2f, 0x2f, 0xff};

static FHANDLE*       file_write_handle = NULL;
static FHANDLE*       file_read_handle  = NULL;

// forward declarations of all functions
/* UI functions*/

static void render_bg(SDL_Renderer* renderer);
static bool on_exit_click(GGWidget* widget, SDL_Event* event);
static void new_filename(char* buf, int size);
static bool on_new_click(GGWidget* widget, SDL_Event* event);
static void vumeter_focus_gained(GGWidget* widget);
static void vumeter_focus_lost(GGWidget* widget);
static void vumeter_grab_dpad(GGWidget* widget);
static void vumeter_release_dpad(GGWidget* widget);
static bool on_record_click(GGWidget* widget, SDL_Event* event);
static bool on_record_stop(GGWidget* widget, SDL_Event* event);
static void handle_recording();
static bool on_play_stop(GGWidget* widget, SDL_Event* event);
static bool on_play_click(GGWidget* widget, SDL_Event* event);
static void handle_playing();
static void check_audio(GGScreen* screen);
static void vumeter_volume_up(GGWidget* widget);
static void vumeter_volume_down(GGWidget* widget);

// utility functions
static void save_audio(int16_t* buf, uint32_t count);
static void ensure_audio_buf_size(u_int32_t lastindex);
static void construct_path(const char* filename, char* buf, int size);
static void create_audio_dir();

int main(int argc, char** argv)
{
    // Open all audio interfaces
    play = playback_open(PLAYBACK_DEVICE, sample_rate, bitdepth);

    if (play == NULL)
    {
        printf("Can't open audio device %s for playback\n", PLAYBACK_DEVICE);
        return ERR_NO_AUDIO;
    }
    mixer_mic = mixer_open(MIXER_MIC);

    // mixer_pcm = mixer_open(MIXER_PCM);
    if (mixer_mic == NULL)
    {
        printf("Can't open mixer %s for playback\n", PLAYBACK_DEVICE);
        return ERR_NO_MIXER;
    }

    capture = capture_open(AUDIO_DEVICE, sample_rate, bitdepth);

    if (capture == NULL)
    {
        printf("Can't open device '%s' for capture\n", CAPTURE_DEVICE);
        return ERR_NO_AUDIO;
    }

    // Correct volume input for the microphone
    mixer_enable_capture(mixer_mic);
    current_volume = mixer_volume(mixer_mic);

    audio_buf1 = malloc(sizeof(int16_t) * AUDIO_BUF_INITIAL_SIZE);
    audio_buf2 = malloc(sizeof(int16_t) * AUDIO_BUF_INITIAL_SIZE);

    if (audio_buf1 == NULL || audio_buf2 == NULL)
    {
        printf("Can't allocate recording buffers\n");
        return ERR_NO_MEMORY;
    }

    // Setup UI
    if (!GGInit(&argc, &argv))
    {
        fprintf(stderr, "Can't initialize (%s)\n", GGLastError());
        return ERR_INIT_SDL;
    }

    screen = GGScreenCreate("Audio Recorder", WINDOW_WIDTH, WINDOW_HEIGHT, FULLSCREEN);

    // Setup callbacks and hooks
    GGScreenSetBackgroundRenderFunc(screen, render_bg);

    GGScreenSetPreEventFunc(screen, check_audio);

    lbl_title = GGLabelCreate(screen, "GCW0 Audio Recorder", 10, 0, 70, 30);
    TTF_Font* font = TTF_OpenFont("assets/DroidSans.ttf", 16);
    TTF_SetFontStyle(font, TTF_STYLE_BOLD);
    GGWidgetSetFont((GGWidget*)lbl_title, font);

    btn_exit = GGImageButtonCreate(screen, "assets/exit.png", 285, 5, 25, 25);
    GGImageButtonSetOnClickFunc(btn_exit, on_exit_click);

    new_filename(output_filename, OUTPUT_FILENAME_SIZE);
    lbl_file = GGLabelCreate(screen, output_filename, 10, 40, 260, 20);

    btn_new = GGButtonCreate(screen, "new", 230, 40, 40, 20);
    GGButtonSetOnClickFunc(btn_new, on_new_click);

    wfw = GGWaveformCreate(screen, 10, 70, 260, 50);

    btn_record = GGImageButtonCreate(screen, "assets/record.png", 10, 130, 30, 30);
    GGImageButtonSetOnClickFunc(btn_record, on_record_click);

    btn_play = GGImageButtonCreate(screen, "assets/play.png", 55, 130, 30, 30);
    GGImageButtonSetOnClickFunc(btn_play, on_play_click);

    btn_stop = GGImageButtonCreate(screen, "assets/stop.png", 55, 130, 30, 30);
    GGWidgetSetVisible((GGWidget*)btn_stop, false);

    btn_replay = GGImageButtonCreate(screen, "assets/replay.png", 95, 130, 30, 30);
    btn_mic    = GGImageButtonCreate(screen, "assets/mic.png", 280, 130, 30, 30);

    // VU Meter
    vumeter                           = GGVUMeterCreate(screen, 280, 40, 30, 80);
    vumeter->widget.focus_gained_func = vumeter_focus_gained;
    vumeter->widget.focus_lost_func   = vumeter_focus_lost;
    vumeter->volume_up_func           = vumeter_volume_up;
    vumeter->volume_down_func         = vumeter_volume_down;
    GGScreenSetGrabDPadCallBack(screen, vumeter_grab_dpad);
    GGScreenSetReleaseDPadCallBack(screen, vumeter_release_dpad);
    float vol = mixer_volume(mixer_mic);
    GGVUMeterSetVolume(vumeter, vol);

    GGVUMeterSetCurrent(vumeter, 1);

    // Helpbar
    helpbar1 = GGHelpBarCreate(screen);
    GGHelpBarSetHelp(helpbar1, GCW_BTN_A, "Record");
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD_ALL, "Navigate");
    //    GGHelpBarSetHelp(helpbar1, GCW_BTN_SELET, "Exit");

    // Set initial widget states
    GGScreenSetFocusWidget(screen, (GGWidget*)btn_record);
    GGWidgetSetEnabled((GGWidget*)btn_play, false);
    GGWidgetSetEnabled((GGWidget*)btn_replay, false);
    GGWidgetSetEnabled((GGWidget*)btn_new, false);

    // playsound();

    GGStart(screen);

    GGScreenDestroy(screen);

    GGQuit();

    free(audio_buf1);
    free(audio_buf2);

    if (file_read_handle)
        file_in_close(file_read_handle);

    if (file_write_handle)
        file_out_close(file_write_handle);

    // These three can't be NULL or the program won't start
    capture_close(capture);
    mixer_close(mixer_mic);
    playback_close(play);

    return 0;
}

/** UI FUNCTIONS **/
static void render_bg(SDL_Renderer* renderer)
{
    // printf("Rendering background\n");
    SDL_SetRenderDrawColor(renderer, dark_gray.r, dark_gray.g, dark_gray.b, dark_gray.a);
    SDL_RenderClear(renderer);
}

static bool on_exit_click(GGWidget* widget, SDL_Event* event)
{
    GGStop();

    return true;
}

static void new_filename(char* buf, int size)
{
    time_t     rawtime;
    struct tm* timeinfo;

    time (&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(buf, size, "AUD_%Y%m%d_%H%M%S.wav", timeinfo);
}

static bool on_new_click(GGWidget* widget, SDL_Event* event)
{
    __attribute__((unused))
    GGButton* button = (GGButton*)widget;

    GGWidgetSetEnabled((GGWidget*)btn_replay, false);
    GGWidgetSetEnabled((GGWidget*)btn_play, false);
    GGWidgetSetEnabled((GGWidget*)btn_record, true);
    GGWidgetSetEnabled((GGWidget*)btn_mic, true);
    GGWidgetSetEnabled((GGWidget*)btn_new, true);

    new_filename(output_filename, OUTPUT_FILENAME_SIZE);
    GGLabelSetLabel(lbl_file, output_filename);
    GGScreenSetFocusWidget(screen, (GGWidget*)btn_record);

    return true;
}

static void vumeter_focus_gained(GGWidget* widget)
{
    GGHelpBarSetHelp(helpbar1, GCW_BTN_SELECT, "Change level");
}

static void vumeter_focus_lost(GGWidget* widget)
{
    // ↔ ↕  : ⇔ ⇕ : ⬄ ⇳
    GGHelpBarSetHelp(helpbar1, GCW_BTN_SELECT, NULL);
}

static void vumeter_grab_dpad(GGWidget* widget)
{
    // ↔ ↕  : ⇔ ⇕ : ⬄ ⇳
    GGHelpBarSetHelp(helpbar1, GCW_BTN_X, "Release");
    GGHelpBarSetHelp(helpbar1, GCW_BTN_SELECT, NULL);
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD_ALL, NULL);
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD_UD, "Inc/Dec Level");
}

static void vumeter_release_dpad(GGWidget* widget)
{
    GGHelpBarSetHelp(helpbar1, GCW_BTN_X, NULL);
    GGHelpBarSetHelp(helpbar1, GCW_BTN_SELECT, "Change level");
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD_ALL, "Navigate");
    GGHelpBarSetHelp(helpbar1, GCW_BTN_DPAD_UD, NULL);
}

/** AUDIO FUNCTIONS **/

static bool on_record_click(GGWidget* widget, SDL_Event* event)
{
    char buf[255];

    capture_prepare(capture);

    // open the file
    create_audio_dir();
    construct_path(output_filename, buf, 255);
    file_write_handle = file_out_open(buf, sample_rate);

    printf("Start recording\n");
    audio_state = RECORDING;

    audio_buf_index1 = 0;

    GGWidgetSetVisible((GGWidget*)btn_play, false);
    GGWidgetSetVisible((GGWidget*)btn_stop, true);
    
    GGWidgetSetEnabled((GGWidget*)btn_new, false);
    GGWidgetSetEnabled((GGWidget*)btn_record, false);
    GGWidgetSetEnabled((GGWidget*)btn_mic, false);
    GGScreenSetFocusWidget(screen, (GGWidget*)btn_stop);
    GGImageButtonSetOnClickFunc(btn_stop, on_record_stop);

    return true;
}

static bool on_record_stop(GGWidget* widget, SDL_Event* event)
{
    GGWidgetSetVisible((GGWidget*)btn_play, true);
    GGWidgetSetVisible((GGWidget*)btn_stop, false);

    GGWidgetSetEnabled((GGWidget*)btn_new, true);
    GGWidgetSetEnabled((GGWidget*)btn_play, true);
    GGWidgetSetEnabled((GGWidget*)btn_mic, true);

    GGScreenSetFocusWidget(screen, (GGWidget*)btn_play);

    printf("Recording done\n");

    capture_stop(capture);

    audio_state = STOPPED;
    
    // write the last buffers to disc
    save_audio(audio_buf1, audio_buf_index1);
    
    file_out_close(file_write_handle);
    file_write_handle = NULL;

    //    GGWaveformSetData(wfw, audio_buf, AUDIO_BUF_INITIAL_SIZE);

    return true;
}

static void handle_recording()
{
    if (!capture_ready(capture))
    {
        printf("Record not ready for fetching data\n");
        return;
    }

    ensure_audio_buf_size(audio_buf_index1 + FRAME_COUNT);

    int actual = capture_record(capture, (void*)&audio_buf1[audio_buf_index1], FRAME_COUNT );

    audio_buf_index1 += actual;
}

static bool on_play_stop(GGWidget* widget, SDL_Event* event)
{
    GGWidgetSetVisible((GGWidget*)btn_play, true);
    GGWidgetSetVisible((GGWidget*)btn_stop, false);

    GGWidgetSetEnabled((GGWidget*)btn_new, true);
    GGWidgetSetEnabled((GGWidget*)btn_mic, true);

    GGScreenSetFocusWidget(screen, (GGWidget*)btn_play);

    audio_state = STOPPED;
    playback_stop(play);

    return true;
}

static bool on_play_click(GGWidget* widget, SDL_Event* event)
{
    // open the file

    int rate;

    char buf[255];
    construct_path(output_filename,buf,255);
    file_read_handle = file_in_open(buf, &rate);

    if (file_read_handle == NULL || rate != 44100)
    {
        fprintf(stderr,"File %s can't be opened or is not 44100 Hz\n", output_filename);
        return true;
    }

    playback_prepare(play);

    printf("Start playback\n");
    audio_state = PLAYING;

    // setup UI
    GGWidgetSetVisible((GGWidget*)btn_play, false);
    GGWidgetSetVisible((GGWidget*)btn_stop, true);
    GGImageButtonSetOnClickFunc(btn_stop, on_play_stop);

    GGWidgetSetEnabled((GGWidget*)btn_mic, false);
    GGWidgetSetEnabled((GGWidget*)btn_replay, true);

    GGScreenSetFocusWidget(screen, (GGWidget*)btn_stop);

    // read first block from file_in_close
    uint32_t actual = file_in_read(file_read_handle, audio_buf1, audio_buf_size);
    audio_buf_count  = actual;
    audio_buf_index1 = 0;

    int count = play->frames;

    if (count > actual)
        count = actual;
    // Send inital frame
    actual = playback_play(play, &audio_buf1[audio_buf_index1], actual);

    audio_buf_index1 += actual;

    return true;
}

static void handle_playing()
{
    if (!playback_ready(play))
    {
        printf("Playback not ready for sending data\n");
        return;
    }

    int count = play->frames;

    // Check if there is still a block left to play in the buffer and adjust if it's smaller than 1 block
    if (audio_buf_index1 + count >= audio_buf_count)
    {
        count = audio_buf_count - audio_buf_index1;
    }
    uint32_t actual = 0;

    if (count > 0)
    {
        // if there are frames then send them.
        // there is still data in the buffer to play.
        actual = playback_play(play, &audio_buf1[audio_buf_index1], count );
    }

    audio_buf_index1 += actual;

    // did we reach the end of the buffer
    if (audio_buf_index1 >= audio_buf_count)
    {
        if (file_read_handle != NULL)
        {
            // were still reading from the file
            // attempt to read again into the buffer
            actual = file_in_read(file_read_handle, audio_buf1, audio_buf_size);

            if (actual == 0)
            {
                // nothing was read. this means nothing more to play
                // nothing more to read from the file either.
                file_in_close(file_read_handle);
                file_read_handle = NULL;
            }
            else
            {
                // if we read data into the buffer
                // then reset the pointers
                audio_buf_count  = actual;
                audio_buf_index1 = 0;
            }
        }
        else
        {
            // The file has nothing to read, now wait for audio to drain
            if (snd_pcm_avail(play->playback_handle) <= 0)
            {
                printf("Audio drained..... done\n");
                on_play_stop(NULL, NULL);
                audio_state = STOPPED;
            }
        }
    }
}

static void check_audio(GGScreen* screen)
{
    switch (audio_state)
    {
        case PLAYING:
            handle_playing();
            break;

        case RECORDING:
            handle_recording();
            break;

        case STOPPED:
        default:
            // do nothing
            break;
    }
}

static void vumeter_volume_up(GGWidget* widget)
{
    GGVUMeter* vumeter = (GGVUMeter*)widget;

    current_volume += VOLUME_STEP;

    if (current_volume > 1)
        current_volume = 1;
    mixer_set_volume(mixer_mic, current_volume);
    GGVUMeterSetVolume(vumeter, current_volume);
}

static void vumeter_volume_down(GGWidget* widget)
{
    GGVUMeter* vumeter = (GGVUMeter*)widget;

    current_volume -= VOLUME_STEP;

    if (current_volume < 0)
        current_volume = 0;
    mixer_set_volume(mixer_mic, current_volume);
    GGVUMeterSetVolume(vumeter, current_volume);
}

static void save_audio(int16_t* buf, uint32_t count)
{
    file_out_write(file_write_handle, buf, count);
}

static void ensure_audio_buf_size(u_int32_t lastindex)
{
    if (lastindex < audio_buf_size)
        return;

    printf("check buf size %u, %u,", audio_buf_size, lastindex);

    // swap buffers and write out the old one to disc

    int16_t* tmp_buf = audio_buf1;
    audio_buf1 = audio_buf2;
    audio_buf2 = tmp_buf;

    u_int32_t tmp_idx = audio_buf_index1;
    audio_buf_index1 = audio_buf_index2;
    audio_buf_index2 = tmp_idx;

    save_audio(tmp_buf, tmp_idx);

    audio_buf_index1 = 0;
}

static void construct_path(const char* filename, char* buf, int size)
{
    char* home = getenv("HOME");

    buf[0] = 0;

    if (home == NULL)
    {
        fprintf(stderr, "$HOME variable not set\n");
        return;
    }

    strncat(buf, home, size);
    strncat(buf, AUDIO_DIR "/", size);
    strncat(buf, filename, size);
}

static void create_audio_dir()
{
    char* home = getenv("HOME");

    if (home == NULL)
    {
        fprintf(stderr, "$HOME variable not set\n");
        return;
    }

    int  err;
    char path[255];
    strncpy(path, home, 255);
    strncat(path, AUDIO_DIR, 10);

    struct stat st = {0};

    if (stat(path, &st) == -1)
    {
        printf("Creating directory: %s\n", path);
        err = mkdir(path, 0700);

        if (err == -1)
        {
            fprintf(stderr, "Failed creating dir %s (%s)\n", path, strerror(errno));
            return;
        }
    }
}