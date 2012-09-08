/* ----------------------------------------------------------------- */
/*           The Japanese TTS System "Open JTalk"                    */
/*           developed by HTS Working Group                          */
/*           http://open-jtalk.sourceforge.net/                      */
/* ----------------------------------------------------------------- */
/*                                                                   */
/*  Copyright (c) 2008-2011  Nagoya Institute of Technology          */
/*                           Department of Computer Science          */
/*                                                                   */
/* All rights reserved.                                              */
/*                                                                   */
/* Redistribution and use in source and binary forms, with or        */
/* without modification, are permitted provided that the following   */
/* conditions are met:                                               */
/*                                                                   */
/* - Redistributions of source code must retain the above copyright  */
/*   notice, this list of conditions and the following disclaimer.   */
/* - Redistributions in binary form must reproduce the above         */
/*   copyright notice, this list of conditions and the following     */
/*   disclaimer in the documentation and/or other materials provided */
/*   with the distribution.                                          */
/* - Neither the name of the HTS working group nor the names of its  */
/*   contributors may be used to endorse or promote products derived */
/*   from this software without specific prior written permission.   */
/*                                                                   */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND            */
/* CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,       */
/* INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF          */
/* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE          */
/* DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS */
/* BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,          */
/* EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,     */
/* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON */
/* ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,   */
/* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY    */
/* OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE           */
/* POSSIBILITY OF SUCH DAMAGE.                                       */
/* ----------------------------------------------------------------- */

/* Main headers */
#include "mecab.h"

extern "C" {
	#include "njd.h"
	#include "jpcommon.h"
	#include "HTS_engine.h"

	/* Sub headers */
	#include "text2mecab.h"
	#include "mecab2njd.h"
	#include "njd_set_pronunciation.h"
	#include "njd_set_digit.h"
	#include "njd_set_accent_phrase.h"
	#include "njd_set_accent_type.h"
	#include "njd_set_unvoiced_vowel.h"
	#include "njd_set_long_vowel.h"
	#include "njd2jpcommon.h"
};
#include "open_jtalkdll.h"

#define MAXBUFLEN 1024
struct OpenJTalk {
   Mecab* mecab;
   NJD njd;
   JPCommon jpcommon;
   HTS_Engine engine;
   char errorout[MAXBUFLEN];
   char **fn_ws_mgc;
   char **fn_ws_lf0;
   char **fn_ws_lpf;
};

const char* OpenJTalk_GetLastError(OpenJTalk** openjtalk)
{
	return (*openjtalk)->errorout;
}

int OpenJTalk_Create(OpenJTalk** openjtalk,Mecab* mecab,int argc,const char **argv)
{
   /* file names of models */
   char *fn_ms_dur = NULL;
   char *fn_ms_mgc = NULL;
   char *fn_ms_lf0 = NULL;
   char *fn_ms_lpf = NULL;

   /* file names of trees */
   char *fn_ts_dur = NULL;
   char *fn_ts_mgc = NULL;
   char *fn_ts_lf0 = NULL;
   char *fn_ts_lpf = NULL;

   /* file names of windows */
   int num_ws_mgc = 0, num_ws_lf0 = 0, num_ws_lpf = 0;

   /* file names of global variance */
   char *fn_ms_gvm = NULL;
   char *fn_ms_gvl = NULL;
   char *fn_ms_gvf = NULL;

   /* file names of global variance trees */
   char *fn_ts_gvm = NULL;
   char *fn_ts_gvl = NULL;
   char *fn_ts_gvf = NULL;

   /* file names of global variance switch */
   char *fn_gv_switch = NULL;

   /* global parameter */
   int sampling_rate = 16000;
   int fperiod = 80;
   double alpha = 0.42;
   int stage = 0;               /* gamma = -1.0/stage */
   double beta = 0.0;
   int audio_buff_size = 1600;
   double uv_threshold = 0.5;
   double gv_weight_mgc = 1.0;
   double gv_weight_lf0 = 1.0;
   double gv_weight_lpf = 1.0;
   HTS_Boolean use_log_gain = FALSE;
   HTS_Boolean use_lpf = FALSE;

   /* alloc OpenJTalk ythis pointer! */
   *openjtalk = (struct OpenJTalk*)malloc(sizeof(struct OpenJTalk));
   (*openjtalk)->mecab = mecab;
   (*openjtalk)->errorout[0] = '\0';
   /* delta window handler for mel-cepstrum */
   (*openjtalk)->fn_ws_mgc = (char **) calloc(argc, sizeof(char *));
   /* delta window handler for log f0 */
   (*openjtalk)->fn_ws_lf0 = (char **) calloc(argc, sizeof(char *));
   /* delta window handler for low-pass filter */
   (*openjtalk)->fn_ws_lpf = (char **) calloc(argc, sizeof(char *));

   /* parse command line */
   if (argc == 1)
   {
       sprintf((*openjtalk)->errorout,"option error!");
       return -1;
   }

   /* read command */
   while (--argc) {
      if (**++argv == '-') {
         switch (*(*argv + 1)) {
         case 't':
            switch (*(*argv + 2)) {
            case 'd':
               fn_ts_dur = (char*)*(++argv);
               break;
            case 'm':
               fn_ts_mgc = (char*)*(++argv);
               break;
            case 'f':
            case 'p':
               fn_ts_lf0 = (char*)*(++argv);
               break;
            case 'l':
               fn_ts_lpf = (char*)*(++argv);
               break;
            default:
               sprintf((*openjtalk)->errorout,"Invalid option '-t%c'.\n", *(*argv + 2));
               return 0;
            }
            --argc;
            break;
         case 'm':
            switch (*(*argv + 2)) {
            case 'd':
               fn_ms_dur = (char*)*(++argv);
               break;
            case 'm':
               fn_ms_mgc = (char*)*(++argv);
               break;
            case 'f':
            case 'p':
               fn_ms_lf0 = (char*)*(++argv);
               break;
            case 'l':
               fn_ms_lpf = (char*)*(++argv);
               break;
            default:
               sprintf((*openjtalk)->errorout,"Invalid option '-m%c'.\n", *(*argv + 2));
               return 0;
            }
            --argc;
            break;
         case 'd':
            switch (*(*argv + 2)) {
            case 'm':
               (*openjtalk)->fn_ws_mgc[num_ws_mgc++] = (char*)*(++argv);
               break;
            case 'f':
            case 'p':
               (*openjtalk)->fn_ws_lf0[num_ws_lf0++] = (char*)*(++argv);
               break;
            case 'l':
               (*openjtalk)->fn_ws_lpf[num_ws_lpf++] = (char*)*(++argv);
               break;
            default:
               sprintf((*openjtalk)->errorout,"Invalid option '-d%c'.\n", *(*argv + 2));
               return 0;
            }
            --argc;
            break;
         case 's':
            sampling_rate = atoi(*++argv);
            --argc;
            break;
         case 'p':
            fperiod = atoi(*++argv);
            --argc;
            break;
         case 'a':
            alpha = atof(*++argv);
            --argc;
            break;
         case 'g':
            stage = atoi(*++argv);
            --argc;
            break;
         case 'l':
            use_log_gain = TRUE;
            break;
         case 'b':
            beta = atof(*++argv);
            --argc;
            break;
         case 'u':
            uv_threshold = atof(*++argv);
            --argc;
            break;
         case 'e':
            switch (*(*argv + 2)) {
            case 'm':
               fn_ts_gvm = (char*)*(++argv);
               break;
            case 'f':
            case 'p':
               fn_ts_gvl = (char*)*(++argv);
               break;
            case 'l':
               fn_ts_gvf = (char*)*(++argv);
               break;
            default:
               sprintf((*openjtalk)->errorout,"Invalid option '-e%c'.\n", *(*argv + 2));
               return 0;
            }
            --argc;
            break;
         case 'c':
            switch (*(*argv + 2)) {
            case 'm':
               fn_ms_gvm = (char*)*(++argv);
               break;
            case 'f':
            case 'p':
               fn_ms_gvl = (char*)*(++argv);
               break;
            case 'l':
               fn_ms_gvf = (char*)*(++argv);
               break;
            default:
               sprintf((*openjtalk)->errorout,"Invalid option '-c%c'.\n", *(*argv + 2));
               return 0;
            }
            --argc;
            break;
         case 'j':
            switch (*(*argv + 2)) {
            case 'm':
               gv_weight_mgc = atof(*(++argv));
               break;
            case 'f':
            case 'p':
               gv_weight_lf0 = atof(*(++argv));
               break;
            case 'l':
               gv_weight_lpf = atof(*(++argv));
               break;
            default:
               sprintf((*openjtalk)->errorout,"Invalid option '-j%c'.\n", *(*argv + 2));
               return 0;
            }
            --argc;
            break;
         case 'k':
            fn_gv_switch = (char*)*++argv;
            --argc;
            break;
         case 'z':
            audio_buff_size = atoi(*++argv);
            --argc;
            break;
         default:
            sprintf((*openjtalk)->errorout,"Invalid option '-%c'.\n", *(*argv + 1));
            return 0;
         }
      }
   }
   /* number of models,trees check */
   if (fn_ms_dur == NULL || fn_ms_mgc == NULL || fn_ms_lf0 == NULL ||
       fn_ts_dur == NULL || fn_ts_mgc == NULL || fn_ts_lf0 == NULL ||
       num_ws_mgc <= 0 || num_ws_lf0 <= 0) {
      sprintf((*openjtalk)->errorout,"Specify models (trees) for each parameter.\n");
      return 0;
   }
   if (fn_ms_lpf != NULL && fn_ts_lpf != NULL && num_ws_lpf > 0)
      use_lpf = TRUE;




   /* initialize */
   NJD_initialize(&(*openjtalk)->njd);
   JPCommon_initialize(&(*openjtalk)->jpcommon);
   if (use_lpf)
      HTS_Engine_initialize(&(*openjtalk)->engine, 3);
   else
      HTS_Engine_initialize(&(*openjtalk)->engine, 2);
   HTS_Engine_set_sampling_rate(&(*openjtalk)->engine, sampling_rate);
   HTS_Engine_set_fperiod(&(*openjtalk)->engine, fperiod);
   HTS_Engine_set_alpha(&(*openjtalk)->engine, alpha);
   HTS_Engine_set_gamma(&(*openjtalk)->engine, stage);
   HTS_Engine_set_log_gain(&(*openjtalk)->engine, use_log_gain);
   HTS_Engine_set_beta(&(*openjtalk)->engine, beta);
   HTS_Engine_set_audio_buff_size(&(*openjtalk)->engine, audio_buff_size);
   HTS_Engine_set_msd_threshold(&(*openjtalk)->engine, 1, uv_threshold);
   HTS_Engine_set_gv_weight(&(*openjtalk)->engine, 0, gv_weight_mgc);
   HTS_Engine_set_gv_weight(&(*openjtalk)->engine, 1, gv_weight_lf0);
   if (use_lpf)
   {
      HTS_Engine_set_gv_weight(&(*openjtalk)->engine, 2, gv_weight_lpf);
   }

   /* Load! */
   HTS_Engine_load_duration_from_fn(&(*openjtalk)->engine, &fn_ms_dur, &fn_ts_dur, 1);
   HTS_Engine_load_parameter_from_fn(&(*openjtalk)->engine, &fn_ms_mgc, &fn_ts_mgc, (*openjtalk)->fn_ws_mgc, 0,
                                     FALSE, num_ws_mgc, 1);
   HTS_Engine_load_parameter_from_fn(&(*openjtalk)->engine, &fn_ms_lf0, &fn_ts_lf0, (*openjtalk)->fn_ws_lf0, 1,
                                     TRUE, num_ws_lf0, 1);
   if (HTS_Engine_get_nstream(&(*openjtalk)->engine) == 3)
      HTS_Engine_load_parameter_from_fn(&(*openjtalk)->engine, &fn_ms_lpf, &fn_ts_lpf, (*openjtalk)->fn_ws_lpf, 2,
                                        FALSE, num_ws_lpf, 1);
   if (fn_ms_gvm != NULL) {
      if (fn_ts_gvm != NULL)
         HTS_Engine_load_gv_from_fn(&(*openjtalk)->engine, &fn_ms_gvm, &fn_ts_gvm, 0, 1);
      else
         HTS_Engine_load_gv_from_fn(&(*openjtalk)->engine, &fn_ms_gvm, NULL, 0, 1);
   }
   if (fn_ms_gvl != NULL) {
      if (fn_ts_gvl != NULL)
         HTS_Engine_load_gv_from_fn(&(*openjtalk)->engine, &fn_ms_gvl, &fn_ts_gvl, 1, 1);
      else
         HTS_Engine_load_gv_from_fn(&(*openjtalk)->engine, &fn_ms_gvl, NULL, 1, 1);
   }
   if (HTS_Engine_get_nstream(&(*openjtalk)->engine) == 3 && fn_ms_gvf != NULL) {
      if (fn_ts_gvf != NULL)
         HTS_Engine_load_gv_from_fn(&(*openjtalk)->engine, &fn_ms_gvf, &fn_ts_gvf, 2, 1);
      else
         HTS_Engine_load_gv_from_fn(&(*openjtalk)->engine, &fn_ms_gvf, NULL, 2, 1);
   }
   if (fn_gv_switch != NULL)
      HTS_Engine_load_gv_switch_from_fn(&(*openjtalk)->engine, fn_gv_switch);

   return 1;
}

int OpenJTalk_synthesis_towav(OpenJTalk** openjtalk,const char* text, const char* wavfilename)
{
   char buff[MAXBUFLEN];
   FILE * wavfp;
   wavfp = fopen(wavfilename,"wb");
   if (!wavfp)
   {
       sprintf((*openjtalk)->errorout,"can not open %s.",wavfilename);
       return 0;
   }

   text2mecab(buff, (char*)text);
   Mecab_analysis((*openjtalk)->mecab, buff);
   mecab2njd(&(*openjtalk)->njd, Mecab_get_feature((*openjtalk)->mecab),
             Mecab_get_size((*openjtalk)->mecab));
   njd_set_pronunciation(&(*openjtalk)->njd);
   njd_set_digit(&(*openjtalk)->njd);
   njd_set_accent_phrase(&(*openjtalk)->njd);
   njd_set_accent_type(&(*openjtalk)->njd);
   njd_set_unvoiced_vowel(&(*openjtalk)->njd);
   njd_set_long_vowel(&(*openjtalk)->njd);
   njd2jpcommon(&(*openjtalk)->jpcommon, &(*openjtalk)->njd);
   JPCommon_make_label(&(*openjtalk)->jpcommon);
   if (JPCommon_get_label_size(&(*openjtalk)->jpcommon) > 2) {
      HTS_Engine_load_label_from_string_list(&(*openjtalk)->engine,
                                             JPCommon_get_label_feature(&(*openjtalk)->jpcommon),
                                             JPCommon_get_label_size(&(*openjtalk)->jpcommon));
      HTS_Engine_create_sstream(&(*openjtalk)->engine);
      HTS_Engine_create_pstream(&(*openjtalk)->engine);
      HTS_Engine_create_gstream(&(*openjtalk)->engine);

      HTS_Engine_save_riff(&(*openjtalk)->engine, wavfp);
/*
      if (wavfp != NULL)
         HTS_Engine_save_riff(&(*openjtalk)->engine, wavfp);
      if (logfp != NULL) {
         fprintf(logfp, "[Text analysis result]\n");
         NJD_fprint(&(*openjtalk)->njd, logfp);
         fprintf(logfp, "\n[Output label]\n");
         HTS_Engine_save_label(&(*openjtalk)->engine, logfp);
         fprintf(logfp, "\n");
         HTS_Engine_save_information(&(*openjtalk)->engine, logfp);
      }
*/
      HTS_Engine_refresh(&(*openjtalk)->engine);
   }
   JPCommon_refresh(&(*openjtalk)->jpcommon);
   NJD_refresh(&(*openjtalk)->njd);
   Mecab_refresh((*openjtalk)->mecab);

   fclose(wavfp);
   return 1;
}


int OpenJTalk_Delete(OpenJTalk** openjtalk)
{
   NJD_clear(&(*openjtalk)->njd);
   JPCommon_clear(&(*openjtalk)->jpcommon);
   HTS_Engine_clear(&(*openjtalk)->engine);

   free( (*openjtalk)->fn_ws_mgc);
   free( (*openjtalk)->fn_ws_lf0);
   free( (*openjtalk)->fn_ws_lpf);
   return 1;
}
