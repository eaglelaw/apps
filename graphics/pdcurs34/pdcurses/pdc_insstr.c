/****************************************************************************
 * apps/graphics/pdcurses/pdc_insstr.c
 * Public Domain Curses
 * RCSID("$Id: insstr.c,v 1.46 2008/07/13 16:08:18 wmcbrine Exp $")
 *
 *   Copyright (C) 2017 Gregory Nutt. All rights reserved.
 *   Adapted by: Gregory Nutt <gnutt@nuttx.org>
 *
 * Adapted from the original public domain pdcurses by Gregory Nutt and
 * released as part of NuttX under the 3-clause BSD license:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name NuttX nor the names of its contributors may be
 *    used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 ****************************************************************************/

/* Name: insstr
 *
 * Synopsis:
 *       int insstr(const char *str);
 *       int insnstr(const char *str, int n);
 *       int winsstr(WINDOW *win, const char *str);
 *       int winsnstr(WINDOW *win, const char *str, int n);
 *       int mvinsstr(int y, int x, const char *str);
 *       int mvinsnstr(int y, int x, const char *str, int n);
 *       int mvwinsstr(WINDOW *win, int y, int x, const char *str);
 *       int mvwinsnstr(WINDOW *win, int y, int x, const char *str, int n);
 *
 *       int ins_wstr(const wchar_t *wstr);
 *       int ins_nwstr(const wchar_t *wstr, int n);
 *       int wins_wstr(WINDOW *win, const wchar_t *wstr);
 *       int wins_nwstr(WINDOW *win, const wchar_t *wstr, int n);
 *       int mvins_wstr(int y, int x, const wchar_t *wstr);
 *       int mvins_nwstr(int y, int x, const wchar_t *wstr, int n);
 *       int mvwins_wstr(WINDOW *win, int y, int x, const wchar_t *wstr);
 *       int mvwins_nwstr(WINDOW *win, int y, int x, const wchar_t *wstr, int n);
 *
 * Description:
 *       The insstr() functions insert a character string into a window
 *       at the current cursor position, by repeatedly calling winsch().
 *       When PDCurses is built with wide-character support enabled, the
 *       narrow-character functions treat the string as a multibyte
 *       string in the current locale, and convert it first. All
 *       characters to the right of the cursor are moved to the right,
 *       with the possibility of the rightmost characters on the line
 *       being lost.  The cursor position does not change (after moving
 *       to y, x, if specified).  The routines with n as the last
 *       argument insert at most n characters; if n is negative, then the
 *       entire string is inserted.
 *
 * Return Value:
 *       All functions return OK on success and ERR on error.
 *
 * Portability                                X/Open    BSD    SYS V
 *       insstr                                  Y       -      4.0
 *       winsstr                                 Y       -      4.0
 *       mvinsstr                                Y       -      4.0
 *       mvwinsstr                               Y       -      4.0
 *       insnstr                                 Y       -      4.0
 *       winsnstr                                Y       -      4.0
 *       mvinsnstr                               Y       -      4.0
 *       mvwinsnstr                              Y       -      4.0
 *       ins_wstr                                Y
 *       wins_wstr                               Y
 *       mvins_wstr                              Y
 *       mvwins_wstr                             Y
 *       ins_nwstr                               Y
 *       wins_nwstr                              Y
 *       mvins_nwstr                             Y
 *       mvwins_nwstr                            Y
 */

/****************************************************************************
 * Included Files
 ****************************************************************************/

#include <string.h>

#include "curspriv.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int winsnstr(WINDOW *win, const char *str, int n)
{
#ifdef CONFIG_PDCURSES_WIDE
  wchar_t wstr[513], *p;
  int i;
#endif
  int len;

  PDC_LOG(("winsnstr() - called: string=\"%s\" n %d\n", str, n));

  if (!win || !str)
    {
      return ERR;
    }

  len = strlen(str);

  if (n < 0 || n < len)
    {
      n = len;
    }

#ifdef CONFIG_PDCURSES_WIDE
  if (n > 512)
    {
      n = 512;
    }

  p = wstr;
  i = 0;

  while (str[i] && i < n)
    {
      int retval = PDC_mbtowc(p, str + i, n - i);

      if (retval <= 0)
        {
          break;
        }

      p++;
      i += retval;
    }

  while (p > wstr)
    {
      if (winsch(win, *--p) == ERR)
        {
          return ERR;
        }
    }
#else
  while (n)
    {
      if (winsch(win, (unsigned char)(str[--n])) == ERR)
        {
          return ERR;
        }
    }
#endif

  return OK;
}

int insstr(const char *str)
{
#ifdef CONFIG_PDCURSES_MULTITHREAD
  FAR struct pdc_context_s *ctx = PDC_ctx();
#endif
  PDC_LOG(("insstr() - called: string=\"%s\"\n", str));

  return winsnstr(stdscr, str, -1);
}

int winsstr(WINDOW *win, const char *str)
{
  PDC_LOG(("winsstr() - called: string=\"%s\"\n", str));

  return winsnstr(win, str, -1);
}

int mvinsstr(int y, int x, const char *str)
{
#ifdef CONFIG_PDCURSES_MULTITHREAD
  FAR struct pdc_context_s *ctx = PDC_ctx();
#endif
  PDC_LOG(("mvinsstr() - called: y %d x %d string=\"%s\"\n", y, x, str));

  if (move(y, x) == ERR)
    {
      return ERR;
    }

  return winsnstr(stdscr, str, -1);
}

int mvwinsstr(WINDOW *win, int y, int x, const char *str)
{
  PDC_LOG(("mvwinsstr() - called: string=\"%s\"\n", str));

  if (wmove(win, y, x) == ERR)
    {
      return ERR;
    }

  return winsnstr(win, str, -1);
}

int insnstr(const char *str, int n)
{
#ifdef CONFIG_PDCURSES_MULTITHREAD
  FAR struct pdc_context_s *ctx = PDC_ctx();
#endif
  PDC_LOG(("insnstr() - called: string=\"%s\" n %d\n", str, n));

  return winsnstr(stdscr, str, n);
}

int mvinsnstr(int y, int x, const char *str, int n)
{
#ifdef CONFIG_PDCURSES_MULTITHREAD
  FAR struct pdc_context_s *ctx = PDC_ctx();
#endif
  PDC_LOG(("mvinsnstr() - called: y %d x %d string=\"%s\" n %d\n",
           y, x, str, n));

  if (move(y, x) == ERR)
    {
      return ERR;
    }

  return winsnstr(stdscr, str, n);
}

int mvwinsnstr(WINDOW *win, int y, int x, const char *str, int n)
{
  PDC_LOG(("mvwinsnstr() - called: y %d x %d string=\"%s\" n %d\n",
           y, x, str, n));

  if (wmove(win, y, x) == ERR)
    {
      return ERR;
    }

  return winsnstr(win, str, n);
}

#ifdef CONFIG_PDCURSES_WIDE
int wins_nwstr(WINDOW *win, const wchar_t *wstr, int n)
{
  const wchar_t *p;
  int len;

  PDC_LOG(("wins_nwstr() - called\n"));

  if (!win || !wstr)
    {
      return ERR;
    }

  for (len = 0, p = wstr; *p; p++)
    {
      len++;
    }

  if (n < 0 || n < len)
    {
      n = len;
    }

  while (n)
    {
      if (winsch(win, wstr[--n]) == ERR)
        {
          return ERR;
        }
    }

  return OK;
}

int ins_wstr(const wchar_t *wstr)
{
#ifdef CONFIG_PDCURSES_MULTITHREAD
  FAR struct pdc_context_s *ctx = PDC_ctx();
#endif
  PDC_LOG(("ins_wstr() - called\n"));

  return wins_nwstr(stdscr, wstr, -1);
}

int wins_wstr(WINDOW *win, const wchar_t *wstr)
{
  PDC_LOG(("wins_wstr() - called\n"));

  return wins_nwstr(win, wstr, -1);
}

int mvins_wstr(int y, int x, const wchar_t *wstr)
{
#ifdef CONFIG_PDCURSES_MULTITHREAD
  FAR struct pdc_context_s *ctx = PDC_ctx();
#endif
  PDC_LOG(("mvins_wstr() - called\n"));

  if (move(y, x) == ERR)
    {
      return ERR;
    }

  return wins_nwstr(stdscr, wstr, -1);
}

int mvwins_wstr(WINDOW *win, int y, int x, const wchar_t *wstr)
{
  PDC_LOG(("mvwinsstr() - called\n"));

  if (wmove(win, y, x) == ERR)
    {
      return ERR;
    }

  return wins_nwstr(win, wstr, -1);
}

int ins_nwstr(const wchar_t *wstr, int n)
{
#ifdef CONFIG_PDCURSES_MULTITHREAD
  FAR struct pdc_context_s *ctx = PDC_ctx();
#endif
  PDC_LOG(("ins_nwstr() - called\n"));

  return wins_nwstr(stdscr, wstr, n);
}

int mvins_nwstr(int y, int x, const wchar_t *wstr, int n)
{
#ifdef CONFIG_PDCURSES_MULTITHREAD
  FAR struct pdc_context_s *ctx = PDC_ctx();
#endif
  PDC_LOG(("mvinsnstr() - called\n"));

  if (move(y, x) == ERR)
    {
      return ERR;
    }

  return wins_nwstr(stdscr, wstr, n);
}

int mvwins_nwstr(WINDOW *win, int y, int x, const wchar_t *wstr, int n)
{
  PDC_LOG(("mvwinsnstr() - called\n"));

  if (wmove(win, y, x) == ERR)
    {
      return ERR;
    }

  return wins_nwstr(win, wstr, n);
}
#endif
