/*
  Copyright (C) 2014 Jolla Ltd.
  Contact: Slava Monich <slava.monich@jolla.com>
  All rights reserved.

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "ClockTheme.h"

ClockTheme* ClockTheme::newDefault()
{
    ClockTheme* theme = new ClockTheme;
    theme->iBackgroundColor.setRgb(228,228,228);
    theme->iBackgroundColor1.setRgb(212,212,212);
    theme->iBackgroundColor2.setRgb(43,43,43);
    theme->iHourMinHandColor.setRgb(43,43,43);
    theme->iSecondHandColor.setRgb(255,0,0);
    theme->iHandShadowColor1.setRgb(228,228,228,0x40);
    theme->iHandShadowColor2.setRgb(228,228,228,0x80);
    return theme;
}

ClockTheme* ClockTheme::newInverted()
{
    ClockTheme* theme = new ClockTheme;
    theme->iBackgroundColor.setRgb(43,43,43);
    theme->iBackgroundColor1.setRgb(86,86,86);
    theme->iBackgroundColor2.setRgb(43,43,43);
    theme->iHourMinHandColor.setRgb(228,228,228);
    theme->iSecondHandColor.setRgb(255,0,0);
    theme->iHandShadowColor1.setRgb(43,43,43,0x80);
    theme->iHandShadowColor2.setRgb(43,43,43,0x40);
    return theme;
}
