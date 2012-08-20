/**
 * @file   wav2mfcc.c
 * 
 * <JA>
 * @brief  ÆÃÄ§ÎÌ¥Ù¥¯¥È¥EMFCC)·ÏÎó¤Î»»½Ğ¡ÊÈó¼Â»ş´ÖÈÇ¡Ë
 *
 * ÆşÎÏ¤µ¤E¿²»À¼ÇÈ·Á¤«¤é¡¤ÆÃÄ§¥Ù¥¯¥È¥EÏÎó¤òÃEĞ¤·¤Ş¤¹. 
 * Julius/Julian¤ÇÃEĞ¤Ç¤­¤EÃÄ§¥Ù¥¯¥È¥EÏ¡¤MFCC ¤ÎÇ¤°Õ¼¡¸µ¿ô¤Î¤â¤Î¤Ç¡¤
 * _0, _E, _D, _A, _Z, _N ¤ÎÇ¤°Õ¤ÎÁÈ¹ç¤E»¤ò¥µ¥İ¡¼¥È¤·¤Ş¤¹. 
 * ¤½¤Î¤Û¤«¡¤ÁE¹¤ä¥Õ¥E¼¥à¥·¥Õ¥È¡¤ÂÓ°è¥«¥Ã¥È¤Ê¤É¤Î¥Ñ¥é¥á¡¼¥¿¤ò»ØÄê¤Ç¤­¤Ş¤¹. 
 * Ç§¼±»ş¤Ë¤Ï¡¤²»¶Á¥â¥Ç¥EÎ¥Ø¥Ã¥À¤È¥Á¥§¥Ã¥¯¤¬¹Ô¤EE¤CMN¤ÎÍ­Ìµ¤Ê¤É
 * ¤¬·èÄê¤µ¤EŞ¤¹. 
 * 
 * ¤³¤³¤Î´Ø¿ô¤Ï¡¤¥Ğ¥Ã¥Õ¥¡¾å¤ËÃßÀÑ¤µ¤E¿²»À¼ÇÈ·Á¥Ç¡¼¥¿¤ò°EÙ¤Ë
 * ÆÃÄ§¥Ù¥¯¥È¥EÏÎó¤ËÊÑ´¹¤¹¤Eâ¤Î¤Ç¡¤¥Õ¥¡¥¤¥EşÎÏ¤Ê¤É¤ËÍÑ¤¤¤é¤EŞ¤¹. 
 * ¥Ş¥¤¥¯ÆşÎÏ¤Ê¤É¤Ç¡¤ÆşÎÏ¤ÈÊ¿¹Ô¤ËÇ§¼±¤ò¹Ô¤¦¾Eç¤Ï¡¤¤³¤³¤Î´Ø¿ô¤Ç¤Ï¤Ê¤¯¡¤
 * realtime-1stpass.c Æâ¤Ç¹Ô¤EEŞ¤¹. 
 * </JA>
 * 
 * <EN>
 * @brief  Calculate feature vector (MFCC) sequence (non on-the-fly ver.)
 *
 * Parameter vector sequence extraction of input speech is done
 * here.  The supported parameter is MFCC, with any combination of
 * all the qualifiers in HTK: _0, _E, _D, _A, _Z, _N.  Acoustic model
 * for recognition should be trained with the same parameter type.
 * You can specify other parameters such as window size, frame shift,
 * high/low frequency cut-off via runtime options.  At startup, Julius
 * will check for the parameter types of acoustic model if it conforms
 * the limitation, and determine whether other additional processing
 * is needed such as Cepstral Mean Normalization.
 *
 * Functions below are used to convert fully buffered whole sentence
 * utterance, and typically used for audio file input.  When input
 * is concurrently processed with recognition process at 1st pass, 
 * in case of microphone input, the MFCC computation will be done
 * within functions in realtime-1stpass.c instead of these.
 * </EN>
 * 
 * @author Akinobu Lee
 * @date   Sun Sep 18 19:40:34 2005
 *
 * $Revision: 1.3 $
 * 
 */
/*
 * Copyright (c) 1991-2011 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2011 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <julius/julius.h>

#include <sys/stat.h>

/** 
 * <JA>
 * ²»À¼ÇÈ·Á¥Ç¡¼¥¿¤«¤EMFCC ¥Ñ¥é¥á¡¼¥¿¤òÃEĞ¤¹¤E
 * ¥¨¥ó¥¸¥ó¥¤¥ó¥¹¥¿¥ó¥¹Æâ¤Î MFCC ·×»»¥¤¥ó¥¹¥¿¥ó¥¹¤´¤È¤Ë¥Ñ¥é¥á¡¼¥¿ÃEĞ¤¬
 * ¹Ô¤EE¤¤½¤E¾¤EÎ mfcc->param ¤Ë³ÊÇ¼¤µ¤EE 
 * 
 * @param speech [in] ²»À¼ÇÈ·Á¥Ç¡¼¥¿
 * @param speechlen [in] @a speech ¤ÎÄ¹¤µ¡ÊÃ±°Ì¡§¥µ¥ó¥×¥Eô¡Ë
 * @param recog [in] ¥¨¥ó¥¸¥ó¥¤¥ó¥¹¥¿¥ó¥¹
 * 
 * @return À®¸ù»ş TRUE, ¥¨¥é¡¼»ş FALSE ¤òÊÖ¤¹. 
 * </JA>
 * <EN>
 * Extract MFCC parameters with sentence CMN from given waveform.
 * Parameters will be computed for each MFCC calculation instance
 * in the engine instance, and stored in mfcc->param for each.
 * 
 * @param speech [in] buffer of speech waveform
 * @param speechlen [in] length of @a speech in samples
 * @param recog [in] engine instance
 * 
 * @return TRUE on success, FALSE on error.
 * </EN>
 *
 * @callgraph
 * @callergraph
 */
boolean
wav2mfcc(SP16 speech[], int speechlen, Recog *recog)
{
  int framenum;
  int len;
  Value *para;
  MFCCCalc *mfcc;

  /* calculate frame length from speech length, frame size and frame shift */
  framenum = (int)((speechlen - recog->jconf->input.framesize) / recog->jconf->input.frameshift) + 1;
  if (framenum < 1) {
    jlog("WARNING: input too short (%d samples), ignored\n", speechlen);
    return FALSE;
  }

  for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {

    if (mfcc->frontend.ssload_filename) {
      /* setup for spectral subtraction using file */
      if (mfcc->frontend.ssbuf == NULL) {
	/* load noise spectrum for spectral subtraction from file (once) */
	if ((mfcc->frontend.ssbuf = new_SS_load_from_file(mfcc->frontend.ssload_filename, &(mfcc->frontend.sslen))) == NULL) {
	  jlog("ERROR: wav2mfcc: failed to read noise spectrum from file \"%s\"\n", mfcc->frontend.ssload_filename);
	  return FALSE;
	}
      }
    }

    if (mfcc->frontend.sscalc) {
      /* compute noise spectrum from head silence for each input */
      len = mfcc->frontend.sscalc_len * recog->jconf->input.sfreq / 1000;
      if (len > speechlen) len = speechlen;
#ifdef SSDEBUG
      jlog("DEBUG: [%d]\n", len);
#endif
      mfcc->frontend.ssbuf = new_SS_calculate(speech, len, &(mfcc->frontend.sslen), mfcc->frontend.mfccwrk_ss, mfcc->para);
    }

  }

  /* compute mfcc from speech file for each mfcc instances */
  for(mfcc=recog->mfcclist;mfcc;mfcc=mfcc->next) {

    para = mfcc->para;

    /* malloc new param */
    param_init_content(mfcc->param);
    if (param_alloc(mfcc->param, framenum, para->veclen) == FALSE) {
      jlog("ERROR: failed to allocate memory for converted parameter vectors\n");
      return FALSE;
    }

    if (mfcc->frontend.ssload_filename || mfcc->frontend.sscalc) {
      /* make link from mfccs to this buffer */
      mfcc->wrk->ssbuf = mfcc->frontend.ssbuf;
      mfcc->wrk->ssbuflen = mfcc->frontend.sslen;
      mfcc->wrk->ss_alpha = mfcc->frontend.ss_alpha;
      mfcc->wrk->ss_floor = mfcc->frontend.ss_floor;
    }
  
    /* make MFCC from speech data */
    if (Wav2MFCC(speech, mfcc->param->parvec, para, speechlen, mfcc->wrk) == FALSE) {
      jlog("ERROR: failed to compute MFCC from input speech\n");
      if (mfcc->frontend.sscalc) {
	free(mfcc->frontend.ssbuf);
	mfcc->frontend.ssbuf = NULL;
      }
      return FALSE;
    }

    /* set miscellaneous parameters */
    mfcc->param->header.samplenum = framenum;
    mfcc->param->header.wshift = para->smp_period * para->frameshift;
    mfcc->param->header.sampsize = para->veclen * sizeof(VECT); /* not compressed */
    mfcc->param->header.samptype = F_MFCC;
    if (para->delta) mfcc->param->header.samptype |= F_DELTA;
    if (para->acc) mfcc->param->header.samptype |= F_ACCL;
    if (para->energy) mfcc->param->header.samptype |= F_ENERGY;
    if (para->c0) mfcc->param->header.samptype |= F_ZEROTH;
    if (para->absesup) mfcc->param->header.samptype |= F_ENERGY_SUP;
    if (para->cmn) mfcc->param->header.samptype |= F_CEPNORM;
    mfcc->param->veclen = para->veclen;
    mfcc->param->samplenum = framenum;

    if (mfcc->frontend.sscalc) {
      free(mfcc->frontend.ssbuf);
      mfcc->frontend.ssbuf = NULL;
    }
  }

  return TRUE;
}

/* end of file */
