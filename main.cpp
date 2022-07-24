#include <stdio.h>
#include <string.h>
#include <Processing.NDI.Lib.h>
#include <unistd.h>  // sleep
#include "myui.h"

int cur_vf = -1;
NDIlib_video_frame_v2_t video_frame[2] = {};
bool opt_verbose = false;

void do_list()
{
  NDIlib_find_instance_t finder = NDIlib_find_create_v2();
  if (!finder) {
    throw std::runtime_error("NDIlib_find_create_v2 failed");
  }

  sleep(1);
  // NDIlib_find_wait_for_sources(pNDI_find, 1000/* One second */);

  uint32_t no_sources = 0;
  const NDIlib_source_t* p_sources = NULL;
  p_sources = NDIlib_find_get_current_sources(finder, &no_sources);

  printf("Found %d Sources:\n", no_sources);
  for (uint32_t i = 0; i < no_sources; i++) {
    printf("  %s  %s\n", p_sources[i].p_ndi_name, p_sources[i].p_url_address);
  }
  printf("\n");
  NDIlib_find_destroy(finder);
}

#define UN_FOURCC(v)  (char)(v&0xff), (char)((v>>8)&0xff), (char)((v>>16)&0xff), (char)((v>>24)&0xff)

void recv_one(NDIlib_recv_instance_t recv, MyUI &ui, int timeout_ms = 100)
{
  const int next_vf = (cur_vf & 1) ^ 1;

//  printf("recv connections: %d\n", NDIlib_recv_get_no_connections(recv));

  switch (NDIlib_recv_capture_v2(recv, &video_frame[next_vf], nullptr, nullptr, timeout_ms)) {
  case NDIlib_frame_type_none:   // No data
    // (don't spam console, even with opt_verbose...)
    break;

  case NDIlib_frame_type_video:  // Video data
    if (cur_vf >= 0) {
      NDIlib_recv_free_video_v2(recv, &video_frame[cur_vf]);
    }
    cur_vf = next_vf;

    if (opt_verbose) {
      printf("Video data received (%dx%d).\n", video_frame[cur_vf].xres, video_frame[cur_vf].yres);
      printf("  FourCC: %c%c%c%c, PAR: %f, ffmt: %02x, fps: %d/%d, timecode: %lld\n",
             UN_FOURCC(video_frame[cur_vf].FourCC),
             video_frame[cur_vf].picture_aspect_ratio,
             video_frame[cur_vf].frame_format_type,
             video_frame[cur_vf].frame_rate_N, video_frame[cur_vf].frame_rate_D,
             video_frame[cur_vf].timecode);    // FIXME: iphone returns timecode (and Sienna NDI Monitor shows it, but we get only 0(?!))  [ --> BUG in linux sdk !? - it works on mac os !]
    }

    // TODO? check FourCC (+ alpha!!), picture_aspect_ratio (= xres/yres [send: 0.0f]?), frame format=progressive [not interlaced!], fps, timecode, stride, metadata, timestamp] ?

    ui.draw(video_frame[cur_vf].p_data, video_frame[cur_vf].line_stride_in_bytes, video_frame[cur_vf].xres, video_frame[cur_vf].yres);
    break;

  case NDIlib_frame_type_audio:  // Audio data
    if (opt_verbose) {
      printf("Audio data received ?!?.\n");
//      printf("Audio data received (%d samples).\n", audio_frame.no_samples);
    }
//    NDIlib_recv_free_audio_v2(recv, &audio_frame);
    break;

  case NDIlib_frame_type_metadata:  // should not happen (nullptr ...);
    if (opt_verbose) {
      printf("Metadata received.\n");
    }
    break;

  case NDIlib_frame_type_error:
    fprintf(stderr, "Error received.\n");  // TODO? message ?
    break;

  case NDIlib_frame_type_status_change:
    if (opt_verbose) {
      printf("Status change received.\n");
      // TODO? ptz, url[, recording]
    }
    break;

  case NDIlib_frame_type_max:
    break;
  }
}

int main(int argc, char **argv)
{
  // Not required, but "correct"
  if (!NDIlib_initialize()) {
    fprintf(stderr, "Cannot run NDI.");
    return 1;
  }

  // -- Argument processing --

  bool opt_usage = false,
       opt_list = false,
       opt_tally_pvw = false, opt_tally_pgm = false,
       opt_gray = false,
       opt_fullscreen = false,
       opt_ipsrc = false;
  const char *opt_src = NULL;

  int opt;
  while ((opt = getopt(argc, argv, "lhpmvgfi")) != -1) {
    switch (opt) {
    case 'l': opt_list = true; break;
    case 'p': opt_tally_pvw = true; break;
    case 'm': opt_tally_pgm = true; break;
    case 'v': opt_verbose = true; break;
    case 'g': opt_gray = true; break;
    case 'f': opt_fullscreen = true; break;
    case 'i': opt_ipsrc = true; break;
    default:
      fprintf(stderr, "Bad argument: %c\n", opt);
    case 'h':
      opt_usage = true;
      break;
    }
  }

  if (!opt_list) {
    if (optind + 1 == argc) {
      opt_src = argv[optind];
    } else {
      opt_usage = true;
    }
  }

  if (opt_usage) {
    fprintf(stderr, "Usage: %s [-l | -h | [-pmvgfi] ndi_source]\n"
                    "  -l  List\n"
                    "  -h  Help\n"
                    "  -p  Send Preview Tally\n"
                    "  -m  Send Program Tally\n"
                    "  -v  Verbose\n"
                    "  -g  Gray background\n"
                    "  -f  Fullscreen\n"
                    "  -i  Treat ndi_source as ip address instead of ndi name\n",
                    argv[0]);
    return 1;
  }

  // --

  if (opt_list) {
    do_list();

  } else {
    MyUI ui{opt_src, opt_gray};

    if (opt_fullscreen) {
      ui.fullscreen(opt_fullscreen);
    }

    NDIlib_recv_create_v3_t rcvt(
      { (opt_ipsrc ? NULL : opt_src), (opt_ipsrc ? opt_src : NULL) },
      NDIlib_recv_color_format_BGRX_BGRA  // (CAVE: linux: RGBX_RGBA is buggy)
//      , NDIlib_recv_bandwidth_highest
//      , false // allow_video_fields_
    );

    NDIlib_recv_instance_t recv = NDIlib_recv_create_v3(&rcvt);
    if (!recv) {
      throw std::runtime_error("NDIlib_recv_create_v3 failed");
    }
#if 0   // TODO? hw accel ?
      NDIlib_metadata_frame_t meta;  // {0, NDIlib_send_timecode_synthesize, (char *)"..."};
      meta.p_data = (char *)"<ndi_hwaccel enabled=\"true\"/>";
      NDIlib_recv_send_metadata(recv, &meta);
#endif

    const NDIlib_tally_t tally{opt_tally_pgm, opt_tally_pvw};
    NDIlib_recv_set_tally(recv, &tally);

    while (ui.run_once()) {
      recv_one(recv, ui);
    }

    // close ui as soon as possible for more responsive feel
    ui.close();

    if (cur_vf >= 0) {
      NDIlib_recv_free_video_v2(recv, &video_frame[cur_vf]);
      cur_vf = -1;
    }
    NDIlib_recv_destroy(recv);
  }

  // --

  // Not required, but nice
  NDIlib_destroy();

  return 0;
}

