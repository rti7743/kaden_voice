/**
 * @file   wav2mfcc.c
 * 
 * <JA>
 * @brief  ��ħ�̥٥��ȥ�EMFCC)����λ��С���»����ǡ�
 *
 * ���Ϥ���E������ȷ����顤��ħ�٥��ȥ�E����ÁEФ��ޤ�. 
 * Julius/Julian��ÁEФǤ���E�ħ�٥��ȥ�Eϡ�MFCC ��Ǥ�ռ������Τ�Τǡ�
 * _0, _E, _D, _A, _Z, _N ��Ǥ�դ��ȹ礁E��򥵥ݡ��Ȥ��ޤ�. 
 * ���Τۤ�����E���ե�E��ॷ�եȡ��Ӱ襫�åȤʤɤΥѥ�᡼�������Ǥ��ޤ�. 
 * ǧ�����ˤϡ�������ǥ�EΥإå��ȥ����å����Ԥ�E�E�CMN��̵ͭ�ʤ�
 * �����ꤵ��Eޤ�. 
 * 
 * �����δؿ��ϡ��Хåե�������Ѥ���E������ȷ��ǡ�����E٤�
 * ��ħ�٥��ȥ�E�����Ѵ�����E�Τǡ��ե�����E��Ϥʤɤ��Ѥ��餁Eޤ�. 
 * �ޥ������Ϥʤɤǡ����Ϥ�ʿ�Ԥ�ǧ����Ԥ���E�ϡ������δؿ��ǤϤʤ���
 * realtime-1stpass.c ��ǹԤ�E�Eޤ�. 
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
 * �����ȷ��ǡ�������EMFCC �ѥ�᡼����ÁEФ���E
 * ���󥸥󥤥󥹥������ MFCC �׻����󥹥��󥹤��Ȥ˥ѥ�᡼��ÁEФ�
 * �Ԥ�E�E�����E���E� mfcc->param �˳�Ǽ����E�E 
 * 
 * @param speech [in] �����ȷ��ǡ���
 * @param speechlen [in] @a speech ��Ĺ����ñ�̡�����ץ�E���
 * @param recog [in] ���󥸥󥤥󥹥���
 * 
 * @return ������ TRUE, ���顼�� FALSE ���֤�. 
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
