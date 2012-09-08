/**
 * @file   m_jconf.c
 * 
 * <JA>
 * @brief  ÀßÄEÕ¥¡¥¤¥EÎÆÉ¤ß¹ş¤ß. 
 *
 * ¥ª¥×¥·¥ç¥ó»ØÄê¤òµ­½Ò¤·¤¿ jconf ÀßÄEÕ¥¡¥¤¥EòÆÉ¤ß¹ş¤ß¤Ş¤¹. 
 * jconf ÀßÄEÕ¥¡¥¤¥Eâ¤Ç¤Ï¡¤¥À¥Ö¥E¯¥©¡¼¥Æ¡¼¥·¥ç¥ó¤Ë¤è¤E¸»úÎó¤Î
 * »ØÄê¡¤¥Ğ¥Ã¥¯¥¹¥é¥Ã¥·¥å¤Ë¤è¤E¸»ú¤Î¥¨¥¹¥±¡¼¥×¤¬¤Ç¤­¤Ş¤¹. 
 * ¤Ş¤¿¡¤³Æ¹Ô¤Ë¤ª¤¤¤Æ '#' °Ê¹ß¤Ï¥¹¥­¥Ã¥×¤µ¤EŞ¤¹. 
 *
 * jconf ÀßÄEÕ¥¡¥¤¥Eâ¤Ç¤Ï¡¤Á´¤Æ¤ÎÁEĞ¥Ñ¥¹¤Ï¡¤¥¢¥×¥E±¡¼¥·¥ç¥ó¤Î
 * ¥«¥Eó¥È¥Ç¥£¥E¯¥È¥ê¤Ç¤Ï¤Ê¤¯¡¤¤½¤Î jconf ¤ÎÂ¸ºß¤¹¤EÇ¥£¥E¯¥È¥ê¤«¤é¤Î
 * ÁEĞ¥Ñ¥¹¤È¤·¤Æ²ò¼á¤µ¤EŞ¤¹. 
 *
 * ¤Ş¤¿¡¤$HOME, ${HOME}, $(HOME), ¤Î·Á¤Ç»ØÄê¤µ¤E¿ÉôÊ¬¤Ë¤Ä¤¤¤Æ
 * ´Ä¶­ÊÑ¿ô¤òÅ¸³«¤Ç¤­¤Ş¤¹. 
 * 
 * </JA>
 * 
 * <EN>
 * @brief  Read a configuration file.
 *
 * These functions are for reading jconf configuration file and set the
 * parameters into jconf structure.  String bracing by double quotation,
 * and escaping character with backslash are supproted.
 * Characters after '#' at each line will be ignored.
 *
 * Note that all relative paths in jconf file are treated as relative
 * to the jconf file, not the run-time current directory.
 *
 * You can expand environment variables in a format of $HOME, ${HOME} or
 * $(HOME) in jconf file.
 *
 * </EN>
 * 
 * @author Akinobu Lee
 * @date   Thu May 12 14:16:18 2005
 *
 * $Revision: 1.8 $
 * 
 */
/*
 * Copyright (c) 1991-2011 Kawahara Lab., Kyoto University
 * Copyright (c) 2000-2005 Shikano Lab., Nara Institute of Science and Technology
 * Copyright (c) 2005-2011 Julius project team, Nagoya Institute of Technology
 * All rights reserved
 */

#include <julius/julius.h>

#if defined(_WIN32) && !defined(__CYGWIN32__)
#include <mbstring.h>
#endif

#define ISTOKEN(A) (A == ' ' || A == '\t' || A == '\n') ///< Determine token characters
#define BUFLEN 512

/** 
 * <JA>
 * @brief  jconf ÍÑ¤Î¹ÔÆÉ¤ß¹ş¤ß¥E¼¥Á¥E *
 * ¥Ğ¥Ã¥¯¥¹¥é¥Ã¥·¥å¤Ë¤è¤E¨¥¹¥±¡¼¥×½èÍı¡¤¤ª¤è¤Ó Mac/Win ¤Î²ş¹Ô¥³¡¼¥É¤Ë
 * ÂĞ±ş¤¹¤E ¶õ¹Ô¤Ï¥¹¥­¥Ã¥×¤µ¤E¤²ş¹Ô¥³¡¼¥É¤Ï¾Ã¤µ¤EE 
 * 
 * @param buf [out] ÆÉ¤ß¹ş¤ó¤À1¹ÔÊ¬¤Î¥Æ¥­¥¹¥È¤ò³ÊÇ¼¤¹¤EĞ¥Ã¥Õ¥¡
 * @param size [in] @a buf ¤ÎÂç¤­¤µ¡Ê¥Ğ¥¤¥È¿ô¡Ë
 * @param fp [in] ¥Õ¥¡¥¤¥Eİ¥¤¥ó¥¿
 * 
 * @return @a buf ¤òÊÖ¤¹. EOF ¤Ç¤³¤EÊ¾åÆşÎÏ¤¬¤Ê¤±¤EĞ NULL ¤òÊÖ¤¹. 
 * </JA>
 * <EN>
 * @brief  line reading function for jconf file.
 *
 * This function has capability of character escaping and newline codes
 * on Win/Mac.  Blank line will be skipped and newline characters will be
 * stripped.
 * 
 * @param buf [out] buffer to store the read text per line
 * @param size [in] size of @a buf in bytes
 * @param fp [in] file pointer
 * 
 * @return @a buf on success, or NULL when encountered EOF and no further input.
 * </EN>
 */
/* added by H.Banno for Windows & Mac */
static char *
fgets_jconf(char *buf, int size, FILE *fp)
{
  int c, prev_c;
  int pos;

  if (fp == NULL) return NULL;
    
  pos = 0;
  c = '\0';
  prev_c = '\0';
  while (1) {
    if (pos >= size) {
      pos--;
      break;
    }

    c = fgetc(fp);
    if (c == EOF) {
      buf[pos] = '\0';
      if (pos <= 0) {
	return NULL;
      } else {
	return buf;
      }
    } else if (c == '\n' || c == '\r') {
      if (c == '\r' && (c = fgetc(fp)) != '\n') { /* for Mac */
	ungetc(c, fp);
      }
      if (prev_c == '\\') {
	pos--;
      } else {
	break;
      }
    } else {
      buf[pos] = c;
      pos++;

#if defined(_WIN32) && !defined(__CYGWIN32__)
      if (c == '\\' && (_ismbblead(prev_c) && _ismbbtrail(c))) {
      c = '\0';
      }
#endif
    }
    prev_c = c;
  }
  buf[pos] = '\0';

  return buf;
}

/** 
 * <JA>
 * @brief  ¥Õ¥¡¥¤¥EÎ¥Ñ¥¹Ì¾¤«¤é¥Ç¥£¥E¯¥È¥E¾¤òÈ´¤­½Ğ¤¹. 
 *
 * ºÇ¸å¤Î '/' ¤Ï»Ä¤µ¤EE 
 * 
 * @param path [i/o] ¥Õ¥¡¥¤¥EÎ¥Ñ¥¹Ì¾¡Ê´Ø¿ôÆâ¤ÇÊÑ¹¹¤µ¤EEË
 * </JA>
 * <EN>
 * @brief  Get directory name from a path name of a file.
 *
 * The trailing slash will be left, and the given buffer will be modified.
 * 
 * @param path [i/o] file path name, will be modified to directory name
 * </EN>
 */
void
get_dirname(char *path)
{
  char *p;
  /* /path/file -> /path/ */
  /* path/file  -> path/  */
  /* /file      -> / */
  /* file       ->  */
  /* ../file    -> ../ */
  p = path + strlen(path) - 1;
  while (*p != '/'
#if defined(_WIN32) && !defined(__CYGWIN32__)
	 && *p != '\\'
#endif
	 && p != path) p--;
  if (p == path && *p != '/') *p = '\0';
  else *(p+1) = '\0';
}

/** 
 * <JA>
 * @brief  ´Ä¶­ÊÑ¿ô¤ÎÅ¸³«
 * 
 * ´Ä¶­ÊÑ¿ô¤òÅ¸³«¤¹¤E $HOME ¤Î·Á¤ÎÊ¸»úÎó¤ò´Ä¶­ÊÑ¿ô¤È¤ß¤Ê¤·¡¤¤½¤ÎÃÍ¤Ç
 * ÃÖ´¹¤¹¤E ÃÖ´¹¤¬µ¯¤³¤Ã¤¿ºİ¤Ë¤Ï¡¤Í¿¤¨¤é¤E¿Ê¸»úÎó¥Ğ¥Ã¥Õ¥¡¤òÆâÉô¤Ç
 * ²òÊE·¡¤¤¢¤é¤¿¤Ë³ä¤EÕ¤±¤é¤E¿¥Ğ¥Ã¥Õ¥¡¤òÊÖ¤¹. 
 *
 * ÊÑ¿ô¤Î»ØÄê¤Ï $HOME, ${HOME}, $(HOME), ¤Î·Á¤Ç»ØÄê¤Ç¤­¤E 
 * $ ¤òÅ¸³«¤·¤¿¤¯¤Ê¤¤¾Eç¤Ï¥Ğ¥Ã¥¯¥¹¥é¥Ã¥·¥E"\" ¤Ç¥¨¥¹¥±¡¼¥×¤Ç¤­¤E 
 * ¤Ş¤¿¥·¥ó¥°¥E¯¥©¡¼¥È "'" ¤Ç³ç¤é¤E¿ÈÏ°Ï¤ÏÅ¸³«¤ò¹Ô¤EÊ¤¤. 
 * 
 * @param str [in] ÂĞ¾İÊ¸»úÎó¡ÊÅ¸³«È¯À¸»ş¤ÏÆâÉô¤Ç free ¤µ¤EEÎ¤ÇÃúÌÕ¡Ë
 * 
 * @return Å¸³«¤¹¤Ù¤­ÂĞ¾İ¤¬¤Ê¤«¤Ã¤¿¾Eç¡¤str ¤¬¤½¤Î¤Ş¤ŞÊÖ¤µ¤EE Å¸³«¤¬¹Ô¤EE¿¾Eç¡¤¤¢¤é¤¿¤Ë³ä¤EÕ¤±¤é¤E¿Å¸³«¸å¤ÎÊ¸»úÎó¤ò´Ş¤à¥Ğ¥Ã¥Õ¥¡¤¬ÊÖ¤µ¤EE 
 * </JA>
 * <EN>
 * @brief  Envronment valuable expansion for a string
 *
 * This function expands environment valuable in a string.  When an
 * expantion occurs, the given buffer will be released inside this
 * function and newly allocated buffer that holds the resulting string
 * will be returned.
 *
 * Environment valuables should be in a form of $HOME, ${HOME} or $(HOME).
 * '$' can be escaped by back slash, and strings enbraced by single quote
 * will be treated as is (no expansion).
 * 
 * @param str [in] target string
 * 
 * @return the str itself when no expansion performed, or newly
 * allocated buffer if expansion occurs.
 * </EN>
 */
static char *
expand_env(char *str)
{
  char *p, *q;
  char *bgn;
  char eb;
  char *target;
  char *envval;
  int target_malloclen;
  int len, n;
  boolean inbrace;
  char env[256];

  /* check if string contains '$' and return immediately if not */
  /* '$' = 36, '\'' = 39 */
  p = str;
  inbrace = FALSE;
  while (*p != '\0') {
    if (*p == 39) {
      if (inbrace == FALSE) {
	inbrace = TRUE;
      } else {
	inbrace = FALSE;
      }
      p++;
      continue;
    }
    if (! inbrace) {
      if (*p == '\\') {
	p++;
	if (*p == '\0') break;
      } else {
	if (*p == 36) break;
      }
    }
    p++;
  }
  if (*p == '\0') return str;

  /* prepare result buffer */
  target_malloclen = strlen(str) * 2;
  target = (char *)mymalloc(target_malloclen);

  p = str;
  q = target;

  /* parsing */
  inbrace = FALSE;
  while (*p != '\0') {

    /* look for next '$' */
    while (*p != '\0') {
      if (*p == 39) {
	if (inbrace == FALSE) {
	  inbrace = TRUE;
	} else {
	  inbrace = FALSE;
	}
	p++;
	continue;
      }
      if (! inbrace) {
	if (*p == '\\') {
	  p++;
	  if (*p == '\0') break;
	} else {
	  if (*p == 36) break;
	}
      }
      *q = *p;
      p++;
      q++;
      n = q - target;
      if (n >= target_malloclen) {
	target_malloclen *= 2;
	target = myrealloc(target, target_malloclen);
	q = target + n;
      }
    }
    if (*p == '\0') {		/* reached end of string */
      *q = '\0';
      break;
    }

    /* move to next */
    p++;

    /* check for brace */
    eb = 0;
    if (*p == '(') {
      eb = ')';
    } else if (*p == '{') {
      eb = '}';
    }

    /* proceed to find env end point and set the env string to env[] */
    if (eb != 0) {
      p++;
      bgn = p;
      while (*p != '\0' && *p != eb) p++;
      if (*p == '\0') {
	jlog("ERROR: failed to expand variable: no end brace: \"%s\"\n", str);
	free(target);
	return str;
      }
    } else {
      bgn = p;
      while (*p == '_'
	     || (*p >= '0' && *p <= '9')
	     || (*p >= 'a' && *p <= 'z')
	     || (*p >= 'A' && *p <= 'Z')) p++;
    }
    len = p - bgn;
    if (len >= 256 - 1) {
      jlog("ERROR: failed to expand variable: too long env name: \"%s\"\n", str);
      free(target);
      return str;
    }
    strncpy(env, bgn, len);
    env[len] = '\0';

    /* get value */
    if ((envval = getenv(env)) == NULL) {
      jlog("ERROR: failed to expand variable: no such variable \"%s\"\n", env);
      free(target);
      return str;
    }

    if (debug2_flag) {		/* for debug */
      jlog("DEBUG: expand $%s to %s\n", env, envval);
    }

    /* paste value to target */
    while(*envval != '\0') {
      *q = *envval;
      q++;
      envval++;
      n = q - target;
      if (n >= target_malloclen) {
	target_malloclen *= 2;
	target = myrealloc(target, target_malloclen);
	q = target + n;
      }
    }

    /* go on to next */
    if (eb != 0) p++;
  }

  free(str);
  return target;
}

/* read-in and parse jconf file and process those using m_options */
/** 
 * <JA>
 * @brief  ¥ª¥×¥·¥ç¥óÊ¸»úÎó¤òÊ¬²ò¤·¤ÆÄÉ²Ã³ÊÇ¼¤¹¤E
 *
 * @param buf [in] Ê¸»úÎE * @param argv [i/o] ¥ª¥×¥·¥ç¥óÎó¤Ø¤Î¥İ¥¤¥ó¥¿
 * @param argc [i/o] ¥ª¥×¥·¥ç¥óÎó¤Î¿ô¤Ø¤Î¥İ¥¤¥ó¥¿
 * @param maxnum [i/o] ¥ª¥×¥·¥ç¥óÎó¤Î³äÉÕºÇÂç¿E * </JA>
 * <EN>
 * @brief  Divide option string into option arguments and append to array.
 *
 * @param buf [in] option string
 * @param argv [i/o] pointer to option array
 * @param argc [i/o] pointer to the length of option array
 * @param maxnum [i/o] pointer to the allocated length of option array
 * </EN>
 */
static void
add_to_arglist(char *buf, char ***argv_ret, int *argc_ret, int *maxnum_ret)
{
  char *p = buf;
  char cpy[BUFLEN];
  char *dst, *dst_from;
  char **argv = *argv_ret;
  int argc = *argc_ret;
  int maxnum = *maxnum_ret;

  dst = cpy;
  while (1) {
    while (*p != '\0' && ISTOKEN(*p)) p++;
    if (*p == '\0') break;
      
    dst_from = dst;
      
    while (*p != '\0' && (!ISTOKEN(*p))) {
      if (*p == '\\') {     /* escape by '\' */
	if (*(++p) == '\0') break;
	*(dst++) = *(p++);
      } else {
	if (*p == '"') { /* quote by "" */
	  p++;
	  while (*p != '\0' && *p != '"') *(dst++) = *(p++);
	  if (*p == '\0') break;
	  p++;
	} else if (*p == '\'') { /* quote by '' */
	  p++;
	  while (*p != '\0' && *p != '\'') *(dst++) = *(p++);
	  if (*p == '\0') break;
	  p++;
	} else if (*p == '#') { /* comment out by '#' */
	  *p = '\0';
	  break;
	} else {		/* other */
	  *(dst++) = *(p++);
	}
      }
    }
    if (dst != dst_from) {
      *dst = '\0'; dst++;
      if ( argc >= maxnum) {
	maxnum += 20;
	argv = (char **)myrealloc(argv, sizeof(char *) * maxnum);
      }
      argv[argc++] = strcpy((char*)mymalloc(strlen(dst_from)+1), dst_from);
    }
  }
  *argv_ret = argv;
  *argc_ret = argc;
  *maxnum_ret = maxnum;
}

/** 
 * <JA>
 * ¥ª¥×¥·¥ç¥ó»ØÄê¤ò´Ş¤àÊ¸»úÎó¤ò²òÀÏ¤·¤ÆÃÍ¤ò¥»¥Ã¥È¤¹¤E
 * ÁEĞ¥Ñ¥¹Ì¾¤Ï¥«¥Eó¥È¤«¤é¤ÎÁEĞ¤È¤·¤Æ°·¤EEE
 * 
 * @param str [in] ¥ª¥×¥·¥ç¥ó»ØÄê¤ò´Ş¤àÊ¸»úÎE * @param jconf [out] ÃÍ¤ò¥»¥Ã¥È¤¹¤Ejconf ÀßÄEÇ¡¼¥¿
 * </JA>
 * <EN>
 * Parse a string and set the specified option values.
 * Relative paths will be treated as relative to current directory.
 * 
 * @param str [in] string which contains options
 * @param jconf [out] global configuration data to be written.
 * </EN>
 *
 * @callgraph
 * @callergraph
 */
boolean
config_string_parse(char *str, Jconf *jconf)
{
  int c_argc;
  char **c_argv;
  int maxnum;
  char buf[BUFLEN];
  char *cdir;
  int i;
  boolean ret;

  jlog("STAT: parsing option string: \"%s\"\n", str);
  
  /* set the content of jconf file into argument list c_argv[1..c_argc-1] */
  maxnum = 20;
  c_argv = (char **)mymalloc(sizeof(char *) * maxnum);
  c_argv[0] = strcpy((char *)mymalloc(7), "string");
  c_argc = 1;
  add_to_arglist(str, &c_argv, &c_argc, &maxnum);
  /* env expansion */
  for (i=1;i<c_argc;i++) {
    c_argv[i] = expand_env(c_argv[i]);
  }
  /* now that options are in c_argv[][], call opt_parse() to process them */
  /* relative paths in string are relative to current */
  ret = opt_parse(c_argc, c_argv, NULL, jconf);

  /* free arguments */
  while (c_argc-- > 0) {
    free(c_argv[c_argc]);
  }
  free(c_argv);

  return(ret);
}

/** 
 * <JA>
 * jconf ÀßÄEÕ¥¡¥¤¥EòÆÉ¤ß¹ş¤ó¤Ç²òÀÏ¤·¡¤ÂĞ±ş¤¹¤Eª¥×¥·¥ç¥ó¤òÀßÄê¤¹¤E
 * ¥ª¥×¥·¥ç¥óÆâ¤ÎÁEĞ¥Ñ¥¹¤Ï¡¢¤½¤Î jconf ÀßÄEÕ¥¡¥¤¥E«¤é¤ÎÁEĞ¤È¤Ê¤E
 * 
 * @param conffile [in] jconf ¥Õ¥¡¥¤¥EÎ¥Ñ¥¹Ì¾
 * @param jconf [out] ÃÍ¤ò¥»¥Ã¥È¤¹¤Ejconf ÀßÄEÇ¡¼¥¿
 * </JA>
 * <EN>
 * Read and parse a jconf file, and set the specified option values.
 * Relative paths in the file will be treated as relative to the file,
 * not the application current.
 * 
 * @param conffile [in] jconf file path name
 * @param jconf [out] global configuration data to be written.
 * </EN>
 *
 * @callgraph
 * @callergraph
 */
boolean
config_file_parse(char *conffile, Jconf *jconf)
{
  int c_argc;
  char **c_argv;
  FILE *fp;
  int maxnum;
  char buf[BUFLEN];
  char *cdir;
  int i;
  boolean ret;

  jlog("STAT: include config: %s\n", conffile);
  
  /* set the content of jconf file into argument list c_argv[1..c_argc-1] */
  /* c_argv[0] will be the original conffile name */
  /* inside jconf file, quoting by ", ' and escape by '\' is supported */
  if ((fp = fopen(conffile, "r")) == NULL) {
    jlog("ERROR: m_jconf: failed to open jconf file: %s\n", conffile);
    return FALSE;
  }
  maxnum = 20;
  c_argv = (char **)mymalloc(sizeof(char *) * maxnum);
  c_argv[0] = strcpy((char *)mymalloc(strlen(conffile)+1), conffile);
  c_argc = 1;
  while (fgets_jconf(buf, BUFLEN, fp) != NULL) {
    if (buf[0] == '\0') continue;
    add_to_arglist(buf, &c_argv, &c_argc, &maxnum);
  }
  if (fclose(fp) == -1) {
    jlog("ERROR: m_jconf: cannot close jconf file\n");
    return FALSE;
  }

  /* env expansion */
  for (i=1;i<c_argc;i++) {
    c_argv[i] = expand_env(c_argv[i]);
  }

  if (debug2_flag) {		/* for debug */
    jlog("DEBUG: args:");
    for (i=1;i<c_argc;i++) jlog(" %s",c_argv[i]);
    jlog("\n");
  }

  /* now that options are in c_argv[][], call opt_parse() to process them */
  /* relative paths in jconf file are relative to the jconf file (not current) */
  cdir = strcpy((char *)mymalloc(strlen(conffile)+1), conffile);
  get_dirname(cdir);
  ret = opt_parse(c_argc, c_argv, (cdir[0] == '\0') ? NULL : cdir, jconf);
  free(cdir);

  /* free arguments */
  while (c_argc-- > 0) {
    free(c_argv[c_argc]);
  }
  free(c_argv);

  return(ret);
}

/* end of file */
