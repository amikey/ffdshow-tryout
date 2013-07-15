#ifndef ISPP_INVOKED
/*
 * (C) 2010-2013 see Authors.txt
 *
 * This file is part of MPC-HC.
 *
 * MPC-HC is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * MPC-HC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#endif // ISPP_INVOKED

#ifndef MPC_VERSION_H
#define MPC_VERSION_H

#ifndef _T
#if !defined(ISPP_INVOKED) && (defined(UNICODE) || defined(_UNICODE))
#define _T(text)    L##text
#else
#define _T(text)    text
#endif
#endif

#define DO_MAKE_STR(x)      _T(#x)
#define MAKE_STR(x)         DO_MAKE_STR(x)

#define MPC_VERSION_MAJOR   1
#define MPC_VERSION_MINOR   7
#define MPC_VERSION_PATCH   0
#define MPC_VERSION_REV     0

#define MPC_COMP_NAME_STR       _T("MPC-HC Team")
#define MPC_COPYRIGHT_STR       _T("Copyright © 2002-2013")

#define MPC_VERSION_NUM         MPC_VERSION_MAJOR,MPC_VERSION_MINOR,MPC_VERSION_PATCH,MPC_VERSION_REV
#define MPC_VERSION_STR         MAKE_STR(MPC_VERSION_MAJOR) _T(".") \
                                MAKE_STR(MPC_VERSION_MINOR) _T(".") \
                                MAKE_STR(MPC_VERSION_PATCH) _T(".") \
                                MAKE_STR(MPC_VERSION_REV)
#define MPC_VERSION_STR_FULL    MPC_VERSION_STR

#endif // MPC_VERSION_H
