//%2003////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2000, 2001, 2002  BMC Software, Hewlett-Packard Development
// Company, L. P., IBM Corp., The Open Group, Tivoli Systems.
// Copyright (c) 2003 BMC Software; Hewlett-Packard Development Company, L. P.;
// IBM Corp.; EMC Corporation, The Open Group.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to
// deal in the Software without restriction, including without limitation the
// rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
// sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// THE ABOVE COPYRIGHT NOTICE AND THIS PERMISSION NOTICE SHALL BE INCLUDED IN
// ALL COPIES OR SUBSTANTIAL PORTIONS OF THE SOFTWARE. THE SOFTWARE IS PROVIDED
// "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT
// LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
// HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
// ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//
//==============================================================================
//
// Author: Mike Brasher (mbrasher@bmc.com)
//
// Modified By:
//
//%/////////////////////////////////////////////////////////////////////////////

#include <cassert>
#include <iostream>
#include <Pegasus/Common/TCPChannel.h>

PEGASUS_USING_PEGASUS;
PEGASUS_USING_STD;

Boolean global_finished = false;

#define D(X) // X

const char LONG_MESSAGE[] = 
"    This is line 0 of a very long message\n"
"    This is line 1 of a very long message\n"
"    This is line 2 of a very long message\n"
"    This is line 3 of a very long message\n"
"    This is line 4 of a very long message\n"
"    This is line 5 of a very long message\n"
"    This is line 6 of a very long message\n"
"    This is line 7 of a very long message\n"
"    This is line 8 of a very long message\n"
"    This is line 9 of a very long message\n"
"    This is line 10 of a very long message\n"
"    This is line 11 of a very long message\n"
"    This is line 12 of a very long message\n"
"    This is line 13 of a very long message\n"
"    This is line 14 of a very long message\n"
"    This is line 15 of a very long message\n"
"    This is line 16 of a very long message\n"
"    This is line 17 of a very long message\n"
"    This is line 18 of a very long message\n"
"    This is line 19 of a very long message\n"
"    This is line 20 of a very long message\n"
"    This is line 21 of a very long message\n"
"    This is line 22 of a very long message\n"
"    This is line 23 of a very long message\n"
"    This is line 24 of a very long message\n"
"    This is line 25 of a very long message\n"
"    This is line 26 of a very long message\n"
"    This is line 27 of a very long message\n"
"    This is line 28 of a very long message\n"
"    This is line 29 of a very long message\n"
"    This is line 30 of a very long message\n"
"    This is line 31 of a very long message\n"
"    This is line 32 of a very long message\n"
"    This is line 33 of a very long message\n"
"    This is line 34 of a very long message\n"
"    This is line 35 of a very long message\n"
"    This is line 36 of a very long message\n"
"    This is line 37 of a very long message\n"
"    This is line 38 of a very long message\n"
"    This is line 39 of a very long message\n"
"    This is line 40 of a very long message\n"
"    This is line 41 of a very long message\n"
"    This is line 42 of a very long message\n"
"    This is line 43 of a very long message\n"
"    This is line 44 of a very long message\n"
"    This is line 45 of a very long message\n"
"    This is line 46 of a very long message\n"
"    This is line 47 of a very long message\n"
"    This is line 48 of a very long message\n"
"    This is line 49 of a very long message\n"
"    This is line 50 of a very long message\n"
"    This is line 51 of a very long message\n"
"    This is line 52 of a very long message\n"
"    This is line 53 of a very long message\n"
"    This is line 54 of a very long message\n"
"    This is line 55 of a very long message\n"
"    This is line 56 of a very long message\n"
"    This is line 57 of a very long message\n"
"    This is line 58 of a very long message\n"
"    This is line 59 of a very long message\n"
"    This is line 60 of a very long message\n"
"    This is line 61 of a very long message\n"
"    This is line 62 of a very long message\n"
"    This is line 63 of a very long message\n"
"    This is line 64 of a very long message\n"
"    This is line 65 of a very long message\n"
"    This is line 66 of a very long message\n"
"    This is line 67 of a very long message\n"
"    This is line 68 of a very long message\n"
"    This is line 69 of a very long message\n"
"    This is line 70 of a very long message\n"
"    This is line 71 of a very long message\n"
"    This is line 72 of a very long message\n"
"    This is line 73 of a very long message\n"
"    This is line 74 of a very long message\n"
"    This is line 75 of a very long message\n"
"    This is line 76 of a very long message\n"
"    This is line 77 of a very long message\n"
"    This is line 78 of a very long message\n"
"    This is line 79 of a very long message\n"
"    This is line 80 of a very long message\n"
"    This is line 81 of a very long message\n"
"    This is line 82 of a very long message\n"
"    This is line 83 of a very long message\n"
"    This is line 84 of a very long message\n"
"    This is line 85 of a very long message\n"
"    This is line 86 of a very long message\n"
"    This is line 87 of a very long message\n"
"    This is line 88 of a very long message\n"
"    This is line 89 of a very long message\n"
"    This is line 90 of a very long message\n"
"    This is line 91 of a very long message\n"
"    This is line 92 of a very long message\n"
"    This is line 93 of a very long message\n"
"    This is line 94 of a very long message\n"
"    This is line 95 of a very long message\n"
"    This is line 96 of a very long message\n"
"    This is line 97 of a very long message\n"
"    This is line 98 of a very long message\n"
"    This is line 99 of a very long message\n"
"    This is line 100 of a very long message\n"
"    This is line 101 of a very long message\n"
"    This is line 102 of a very long message\n"
"    This is line 103 of a very long message\n"
"    This is line 104 of a very long message\n"
"    This is line 105 of a very long message\n"
"    This is line 106 of a very long message\n"
"    This is line 107 of a very long message\n"
"    This is line 108 of a very long message\n"
"    This is line 109 of a very long message\n"
"    This is line 110 of a very long message\n"
"    This is line 111 of a very long message\n"
"    This is line 112 of a very long message\n"
"    This is line 113 of a very long message\n"
"    This is line 114 of a very long message\n"
"    This is line 115 of a very long message\n"
"    This is line 116 of a very long message\n"
"    This is line 117 of a very long message\n"
"    This is line 118 of a very long message\n"
"    This is line 119 of a very long message\n"
"    This is line 120 of a very long message\n"
"    This is line 121 of a very long message\n"
"    This is line 122 of a very long message\n"
"    This is line 123 of a very long message\n"
"    This is line 124 of a very long message\n"
"    This is line 125 of a very long message\n"
"    This is line 126 of a very long message\n"
"    This is line 127 of a very long message\n"
"    This is line 128 of a very long message\n"
"    This is line 129 of a very long message\n"
"    This is line 130 of a very long message\n"
"    This is line 131 of a very long message\n"
"    This is line 132 of a very long message\n"
"    This is line 133 of a very long message\n"
"    This is line 134 of a very long message\n"
"    This is line 135 of a very long message\n"
"    This is line 136 of a very long message\n"
"    This is line 137 of a very long message\n"
"    This is line 138 of a very long message\n"
"    This is line 139 of a very long message\n"
"    This is line 140 of a very long message\n"
"    This is line 141 of a very long message\n"
"    This is line 142 of a very long message\n"
"    This is line 143 of a very long message\n"
"    This is line 144 of a very long message\n"
"    This is line 145 of a very long message\n"
"    This is line 146 of a very long message\n"
"    This is line 147 of a very long message\n"
"    This is line 148 of a very long message\n"
"    This is line 149 of a very long message\n"
"    This is line 150 of a very long message\n"
"    This is line 151 of a very long message\n"
"    This is line 152 of a very long message\n"
"    This is line 153 of a very long message\n"
"    This is line 154 of a very long message\n"
"    This is line 155 of a very long message\n"
"    This is line 156 of a very long message\n"
"    This is line 157 of a very long message\n"
"    This is line 158 of a very long message\n"
"    This is line 159 of a very long message\n"
"    This is line 160 of a very long message\n"
"    This is line 161 of a very long message\n"
"    This is line 162 of a very long message\n"
"    This is line 163 of a very long message\n"
"    This is line 164 of a very long message\n"
"    This is line 165 of a very long message\n"
"    This is line 166 of a very long message\n"
"    This is line 167 of a very long message\n"
"    This is line 168 of a very long message\n"
"    This is line 169 of a very long message\n"
"    This is line 170 of a very long message\n"
"    This is line 171 of a very long message\n"
"    This is line 172 of a very long message\n"
"    This is line 173 of a very long message\n"
"    This is line 174 of a very long message\n"
"    This is line 175 of a very long message\n"
"    This is line 176 of a very long message\n"
"    This is line 177 of a very long message\n"
"    This is line 178 of a very long message\n"
"    This is line 179 of a very long message\n"
"    This is line 180 of a very long message\n"
"    This is line 181 of a very long message\n"
"    This is line 182 of a very long message\n"
"    This is line 183 of a very long message\n"
"    This is line 184 of a very long message\n"
"    This is line 185 of a very long message\n"
"    This is line 186 of a very long message\n"
"    This is line 187 of a very long message\n"
"    This is line 188 of a very long message\n"
"    This is line 189 of a very long message\n"
"    This is line 190 of a very long message\n"
"    This is line 191 of a very long message\n"
"    This is line 192 of a very long message\n"
"    This is line 193 of a very long message\n"
"    This is line 194 of a very long message\n"
"    This is line 195 of a very long message\n"
"    This is line 196 of a very long message\n"
"    This is line 197 of a very long message\n"
"    This is line 198 of a very long message\n"
"    This is line 199 of a very long message\n"
"    This is line 200 of a very long message\n"
"    This is line 201 of a very long message\n"
"    This is line 202 of a very long message\n"
"    This is line 203 of a very long message\n"
"    This is line 204 of a very long message\n"
"    This is line 205 of a very long message\n"
"    This is line 206 of a very long message\n"
"    This is line 207 of a very long message\n"
"    This is line 208 of a very long message\n"
"    This is line 209 of a very long message\n"
"    This is line 210 of a very long message\n"
"    This is line 211 of a very long message\n"
"    This is line 212 of a very long message\n"
"    This is line 213 of a very long message\n"
"    This is line 214 of a very long message\n"
"    This is line 215 of a very long message\n"
"    This is line 216 of a very long message\n"
"    This is line 217 of a very long message\n"
"    This is line 218 of a very long message\n"
"    This is line 219 of a very long message\n"
"    This is line 220 of a very long message\n"
"    This is line 221 of a very long message\n"
"    This is line 222 of a very long message\n"
"    This is line 223 of a very long message\n"
"    This is line 224 of a very long message\n"
"    This is line 225 of a very long message\n"
"    This is line 226 of a very long message\n"
"    This is line 227 of a very long message\n"
"    This is line 228 of a very long message\n"
"    This is line 229 of a very long message\n"
"    This is line 230 of a very long message\n"
"    This is line 231 of a very long message\n"
"    This is line 232 of a very long message\n"
"    This is line 233 of a very long message\n"
"    This is line 234 of a very long message\n"
"    This is line 235 of a very long message\n"
"    This is line 236 of a very long message\n"
"    This is line 237 of a very long message\n"
"    This is line 238 of a very long message\n"
"    This is line 239 of a very long message\n"
"    This is line 240 of a very long message\n"
"    This is line 241 of a very long message\n"
"    This is line 242 of a very long message\n"
"    This is line 243 of a very long message\n"
"    This is line 244 of a very long message\n"
"    This is line 245 of a very long message\n"
"    This is line 246 of a very long message\n"
"    This is line 247 of a very long message\n"
"    This is line 248 of a very long message\n"
"    This is line 249 of a very long message\n"
"    This is line 250 of a very long message\n"
"    This is line 251 of a very long message\n"
"    This is line 252 of a very long message\n"
"    This is line 253 of a very long message\n"
"    This is line 254 of a very long message\n"
"    This is line 255 of a very long message\n"
"    This is line 256 of a very long message\n"
"    This is line 257 of a very long message\n"
"    This is line 258 of a very long message\n"
"    This is line 259 of a very long message\n"
"    This is line 260 of a very long message\n"
"    This is line 261 of a very long message\n"
"    This is line 262 of a very long message\n"
"    This is line 263 of a very long message\n"
"    This is line 264 of a very long message\n"
"    This is line 265 of a very long message\n"
"    This is line 266 of a very long message\n"
"    This is line 267 of a very long message\n"
"    This is line 268 of a very long message\n"
"    This is line 269 of a very long message\n"
"    This is line 270 of a very long message\n"
"    This is line 271 of a very long message\n"
"    This is line 272 of a very long message\n"
"    This is line 273 of a very long message\n"
"    This is line 274 of a very long message\n"
"    This is line 275 of a very long message\n"
"    This is line 276 of a very long message\n"
"    This is line 277 of a very long message\n"
"    This is line 278 of a very long message\n"
"    This is line 279 of a very long message\n"
"    This is line 280 of a very long message\n"
"    This is line 281 of a very long message\n"
"    This is line 282 of a very long message\n"
"    This is line 283 of a very long message\n"
"    This is line 284 of a very long message\n"
"    This is line 285 of a very long message\n"
"    This is line 286 of a very long message\n"
"    This is line 287 of a very long message\n"
"    This is line 288 of a very long message\n"
"    This is line 289 of a very long message\n"
"    This is line 290 of a very long message\n"
"    This is line 291 of a very long message\n"
"    This is line 292 of a very long message\n"
"    This is line 293 of a very long message\n"
"    This is line 294 of a very long message\n"
"    This is line 295 of a very long message\n"
"    This is line 296 of a very long message\n"
"    This is line 297 of a very long message\n"
"    This is line 298 of a very long message\n"
"    This is line 299 of a very long message\n"
"    This is line 300 of a very long message\n"
"    This is line 301 of a very long message\n"
"    This is line 302 of a very long message\n"
"    This is line 303 of a very long message\n"
"    This is line 304 of a very long message\n"
"    This is line 305 of a very long message\n"
"    This is line 306 of a very long message\n"
"    This is line 307 of a very long message\n"
"    This is line 308 of a very long message\n"
"    This is line 309 of a very long message\n"
"    This is line 310 of a very long message\n"
"    This is line 311 of a very long message\n"
"    This is line 312 of a very long message\n"
"    This is line 313 of a very long message\n"
"    This is line 314 of a very long message\n"
"    This is line 315 of a very long message\n"
"    This is line 316 of a very long message\n"
"    This is line 317 of a very long message\n"
"    This is line 318 of a very long message\n"
"    This is line 319 of a very long message\n"
"    This is line 320 of a very long message\n"
"    This is line 321 of a very long message\n"
"    This is line 322 of a very long message\n"
"    This is line 323 of a very long message\n"
"    This is line 324 of a very long message\n"
"    This is line 325 of a very long message\n"
"    This is line 326 of a very long message\n"
"    This is line 327 of a very long message\n"
"    This is line 328 of a very long message\n"
"    This is line 329 of a very long message\n"
"    This is line 330 of a very long message\n"
"    This is line 331 of a very long message\n"
"    This is line 332 of a very long message\n"
"    This is line 333 of a very long message\n"
"    This is line 334 of a very long message\n"
"    This is line 335 of a very long message\n"
"    This is line 336 of a very long message\n"
"    This is line 337 of a very long message\n"
"    This is line 338 of a very long message\n"
"    This is line 339 of a very long message\n"
"    This is line 340 of a very long message\n"
"    This is line 341 of a very long message\n"
"    This is line 342 of a very long message\n"
"    This is line 343 of a very long message\n"
"    This is line 344 of a very long message\n"
"    This is line 345 of a very long message\n"
"    This is line 346 of a very long message\n"
"    This is line 347 of a very long message\n"
"    This is line 348 of a very long message\n"
"    This is line 349 of a very long message\n"
"    This is line 350 of a very long message\n"
"    This is line 351 of a very long message\n"
"    This is line 352 of a very long message\n"
"    This is line 353 of a very long message\n"
"    This is line 354 of a very long message\n"
"    This is line 355 of a very long message\n"
"    This is line 356 of a very long message\n"
"    This is line 357 of a very long message\n"
"    This is line 358 of a very long message\n"
"    This is line 359 of a very long message\n"
"    This is line 360 of a very long message\n"
"    This is line 361 of a very long message\n"
"    This is line 362 of a very long message\n"
"    This is line 363 of a very long message\n"
"    This is line 364 of a very long message\n"
"    This is line 365 of a very long message\n"
"    This is line 366 of a very long message\n"
"    This is line 367 of a very long message\n"
"    This is line 368 of a very long message\n"
"    This is line 369 of a very long message\n"
"    This is line 370 of a very long message\n"
"    This is line 371 of a very long message\n"
"    This is line 372 of a very long message\n"
"    This is line 373 of a very long message\n"
"    This is line 374 of a very long message\n"
"    This is line 375 of a very long message\n"
"    This is line 376 of a very long message\n"
"    This is line 377 of a very long message\n"
"    This is line 378 of a very long message\n"
"    This is line 379 of a very long message\n"
"    This is line 380 of a very long message\n"
"    This is line 381 of a very long message\n"
"    This is line 382 of a very long message\n"
"    This is line 383 of a very long message\n"
"    This is line 384 of a very long message\n"
"    This is line 385 of a very long message\n"
"    This is line 386 of a very long message\n"
"    This is line 387 of a very long message\n"
"    This is line 388 of a very long message\n"
"    This is line 389 of a very long message\n"
"    This is line 390 of a very long message\n"
"    This is line 391 of a very long message\n"
"    This is line 392 of a very long message\n"
"    This is line 393 of a very long message\n"
"    This is line 394 of a very long message\n"
"    This is line 395 of a very long message\n"
"    This is line 396 of a very long message\n"
"    This is line 397 of a very long message\n"
"    This is line 398 of a very long message\n"
"    This is line 399 of a very long message\n"
"    This is line 400 of a very long message\n"
"    This is line 401 of a very long message\n"
"    This is line 402 of a very long message\n"
"    This is line 403 of a very long message\n"
"    This is line 404 of a very long message\n"
"    This is line 405 of a very long message\n"
"    This is line 406 of a very long message\n"
"    This is line 407 of a very long message\n"
"    This is line 408 of a very long message\n"
"    This is line 409 of a very long message\n"
"    This is line 410 of a very long message\n"
"    This is line 411 of a very long message\n"
"    This is line 412 of a very long message\n"
"    This is line 413 of a very long message\n"
"    This is line 414 of a very long message\n"
"    This is line 415 of a very long message\n"
"    This is line 416 of a very long message\n"
"    This is line 417 of a very long message\n"
"    This is line 418 of a very long message\n"
"    This is line 419 of a very long message\n"
"    This is line 420 of a very long message\n"
"    This is line 421 of a very long message\n"
"    This is line 422 of a very long message\n"
"    This is line 423 of a very long message\n"
"    This is line 424 of a very long message\n"
"    This is line 425 of a very long message\n"
"    This is line 426 of a very long message\n"
"    This is line 427 of a very long message\n"
"    This is line 428 of a very long message\n"
"    This is line 429 of a very long message\n"
"    This is line 430 of a very long message\n"
"    This is line 431 of a very long message\n"
"    This is line 432 of a very long message\n"
"    This is line 433 of a very long message\n"
"    This is line 434 of a very long message\n"
"    This is line 435 of a very long message\n"
"    This is line 436 of a very long message\n"
"    This is line 437 of a very long message\n"
"    This is line 438 of a very long message\n"
"    This is line 439 of a very long message\n"
"    This is line 440 of a very long message\n"
"    This is line 441 of a very long message\n"
"    This is line 442 of a very long message\n"
"    This is line 443 of a very long message\n"
"    This is line 444 of a very long message\n"
"    This is line 445 of a very long message\n"
"    This is line 446 of a very long message\n"
"    This is line 447 of a very long message\n"
"    This is line 448 of a very long message\n"
"    This is line 449 of a very long message\n"
"    This is line 450 of a very long message\n"
"    This is line 451 of a very long message\n"
"    This is line 452 of a very long message\n"
"    This is line 453 of a very long message\n"
"    This is line 454 of a very long message\n"
"    This is line 455 of a very long message\n"
"    This is line 456 of a very long message\n"
"    This is line 457 of a very long message\n"
"    This is line 458 of a very long message\n"
"    This is line 459 of a very long message\n"
"    This is line 460 of a very long message\n"
"    This is line 461 of a very long message\n"
"    This is line 462 of a very long message\n"
"    This is line 463 of a very long message\n"
"    This is line 464 of a very long message\n"
"    This is line 465 of a very long message\n"
"    This is line 466 of a very long message\n"
"    This is line 467 of a very long message\n"
"    This is line 468 of a very long message\n"
"    This is line 469 of a very long message\n"
"    This is line 470 of a very long message\n"
"    This is line 471 of a very long message\n"
"    This is line 472 of a very long message\n"
"    This is line 473 of a very long message\n"
"    This is line 474 of a very long message\n"
"    This is line 475 of a very long message\n"
"    This is line 476 of a very long message\n"
"    This is line 477 of a very long message\n"
"    This is line 478 of a very long message\n"
"    This is line 479 of a very long message\n"
"    This is line 480 of a very long message\n"
"    This is line 481 of a very long message\n"
"    This is line 482 of a very long message\n"
"    This is line 483 of a very long message\n"
"    This is line 484 of a very long message\n"
"    This is line 485 of a very long message\n"
"    This is line 486 of a very long message\n"
"    This is line 487 of a very long message\n"
"    This is line 488 of a very long message\n"
"    This is line 489 of a very long message\n"
"    This is line 490 of a very long message\n"
"    This is line 491 of a very long message\n"
"    This is line 492 of a very long message\n"
"    This is line 493 of a very long message\n"
"    This is line 494 of a very long message\n"
"    This is line 495 of a very long message\n"
"    This is line 496 of a very long message\n"
"    This is line 497 of a very long message\n"
"    This is line 498 of a very long message\n"
"    This is line 499 of a very long message\n"
"    This is line 500 of a very long message\n"
"    This is line 501 of a very long message\n"
"    This is line 502 of a very long message\n"
"    This is line 503 of a very long message\n"
"    This is line 504 of a very long message\n"
"    This is line 505 of a very long message\n"
"    This is line 506 of a very long message\n"
"    This is line 507 of a very long message\n"
"    This is line 508 of a very long message\n"
"    This is line 509 of a very long message\n"
"    This is line 510 of a very long message\n"
"    This is line 511 of a very long message\n"
"    This is line 512 of a very long message\n"
"    This is line 513 of a very long message\n"
"    This is line 514 of a very long message\n"
"    This is line 515 of a very long message\n"
"    This is line 516 of a very long message\n"
"    This is line 517 of a very long message\n"
"    This is line 518 of a very long message\n"
"    This is line 519 of a very long message\n"
"    This is line 520 of a very long message\n"
"    This is line 521 of a very long message\n"
"    This is line 522 of a very long message\n"
"    This is line 523 of a very long message\n"
"    This is line 524 of a very long message\n"
"    This is line 525 of a very long message\n"
"    This is line 526 of a very long message\n"
"    This is line 527 of a very long message\n"
"    This is line 528 of a very long message\n"
"    This is line 529 of a very long message\n"
"    This is line 530 of a very long message\n"
"    This is line 531 of a very long message\n"
"    This is line 532 of a very long message\n"
"    This is line 533 of a very long message\n"
"    This is line 534 of a very long message\n"
"    This is line 535 of a very long message\n"
"    This is line 536 of a very long message\n"
"    This is line 537 of a very long message\n"
"    This is line 538 of a very long message\n"
"    This is line 539 of a very long message\n"
"    This is line 540 of a very long message\n"
"    This is line 541 of a very long message\n"
"    This is line 542 of a very long message\n"
"    This is line 543 of a very long message\n"
"    This is line 544 of a very long message\n"
"    This is line 545 of a very long message\n"
"    This is line 546 of a very long message\n"
"    This is line 547 of a very long message\n"
"    This is line 548 of a very long message\n"
"    This is line 549 of a very long message\n"
"    This is line 550 of a very long message\n"
"    This is line 551 of a very long message\n"
"    This is line 552 of a very long message\n"
"    This is line 553 of a very long message\n"
"    This is line 554 of a very long message\n"
"    This is line 555 of a very long message\n"
"    This is line 556 of a very long message\n"
"    This is line 557 of a very long message\n"
"    This is line 558 of a very long message\n"
"    This is line 559 of a very long message\n"
"    This is line 560 of a very long message\n"
"    This is line 561 of a very long message\n"
"    This is line 562 of a very long message\n"
"    This is line 563 of a very long message\n"
"    This is line 564 of a very long message\n"
"    This is line 565 of a very long message\n"
"    This is line 566 of a very long message\n"
"    This is line 567 of a very long message\n"
"    This is line 568 of a very long message\n"
"    This is line 569 of a very long message\n"
"    This is line 570 of a very long message\n"
"    This is line 571 of a very long message\n"
"    This is line 572 of a very long message\n"
"    This is line 573 of a very long message\n"
"    This is line 574 of a very long message\n"
"    This is line 575 of a very long message\n"
"    This is line 576 of a very long message\n"
"    This is line 577 of a very long message\n"
"    This is line 578 of a very long message\n"
"    This is line 579 of a very long message\n"
"    This is line 580 of a very long message\n"
"    This is line 581 of a very long message\n"
"    This is line 582 of a very long message\n"
"    This is line 583 of a very long message\n"
"    This is line 584 of a very long message\n"
"    This is line 585 of a very long message\n"
"    This is line 586 of a very long message\n"
"    This is line 587 of a very long message\n"
"    This is line 588 of a very long message\n"
"    This is line 589 of a very long message\n"
"    This is line 590 of a very long message\n"
"    This is line 591 of a very long message\n"
"    This is line 592 of a very long message\n"
"    This is line 593 of a very long message\n"
"    This is line 594 of a very long message\n"
"    This is line 595 of a very long message\n"
"    This is line 596 of a very long message\n"
"    This is line 597 of a very long message\n"
"    This is line 598 of a very long message\n"
"    This is line 599 of a very long message\n"
"    This is line 600 of a very long message\n"
"    This is line 601 of a very long message\n"
"    This is line 602 of a very long message\n"
"    This is line 603 of a very long message\n"
"    This is line 604 of a very long message\n"
"    This is line 605 of a very long message\n"
"    This is line 606 of a very long message\n"
"    This is line 607 of a very long message\n"
"    This is line 608 of a very long message\n"
"    This is line 609 of a very long message\n"
"    This is line 610 of a very long message\n"
"    This is line 611 of a very long message\n"
"    This is line 612 of a very long message\n"
"    This is line 613 of a very long message\n"
"    This is line 614 of a very long message\n"
"    This is line 615 of a very long message\n"
"    This is line 616 of a very long message\n"
"    This is line 617 of a very long message\n"
"    This is line 618 of a very long message\n"
"    This is line 619 of a very long message\n"
"    This is line 620 of a very long message\n"
"    This is line 621 of a very long message\n"
"    This is line 622 of a very long message\n"
"    This is line 623 of a very long message\n"
"    This is line 624 of a very long message\n"
"    This is line 625 of a very long message\n"
"    This is line 626 of a very long message\n"
"    This is line 627 of a very long message\n"
"    This is line 628 of a very long message\n"
"    This is line 629 of a very long message\n"
"    This is line 630 of a very long message\n"
"    This is line 631 of a very long message\n"
"    This is line 632 of a very long message\n"
"    This is line 633 of a very long message\n"
"    This is line 634 of a very long message\n"
"    This is line 635 of a very long message\n"
"    This is line 636 of a very long message\n"
"    This is line 637 of a very long message\n"
"    This is line 638 of a very long message\n"
"    This is line 639 of a very long message\n"
"    This is line 640 of a very long message\n"
"    This is line 641 of a very long message\n"
"    This is line 642 of a very long message\n"
"    This is line 643 of a very long message\n"
"    This is line 644 of a very long message\n"
"    This is line 645 of a very long message\n"
"    This is line 646 of a very long message\n"
"    This is line 647 of a very long message\n"
"    This is line 648 of a very long message\n"
"    This is line 649 of a very long message\n"
"    This is line 650 of a very long message\n"
"    This is line 651 of a very long message\n"
"    This is line 652 of a very long message\n"
"    This is line 653 of a very long message\n"
"    This is line 654 of a very long message\n"
"    This is line 655 of a very long message\n"
"    This is line 656 of a very long message\n"
"    This is line 657 of a very long message\n"
"    This is line 658 of a very long message\n"
"    This is line 659 of a very long message\n"
"    This is line 660 of a very long message\n"
"    This is line 661 of a very long message\n"
"    This is line 662 of a very long message\n"
"    This is line 663 of a very long message\n"
"    This is line 664 of a very long message\n"
"    This is line 665 of a very long message\n"
"    This is line 666 of a very long message\n"
"    This is line 667 of a very long message\n"
"    This is line 668 of a very long message\n"
"    This is line 669 of a very long message\n"
"    This is line 670 of a very long message\n"
"    This is line 671 of a very long message\n"
"    This is line 672 of a very long message\n"
"    This is line 673 of a very long message\n"
"    This is line 674 of a very long message\n"
"    This is line 675 of a very long message\n"
"    This is line 676 of a very long message\n"
"    This is line 677 of a very long message\n"
"    This is line 678 of a very long message\n"
"    This is line 679 of a very long message\n"
"    This is line 680 of a very long message\n"
"    This is line 681 of a very long message\n"
"    This is line 682 of a very long message\n"
"    This is line 683 of a very long message\n"
"    This is line 684 of a very long message\n"
"    This is line 685 of a very long message\n"
"    This is line 686 of a very long message\n"
"    This is line 687 of a very long message\n"
"    This is line 688 of a very long message\n";

class ClientHandler : public ChannelHandler
{
public:

    ClientHandler()
    {
	D( cout << "ClientHandler::ClientHandler()" << endl; )
    }

    virtual ~ClientHandler()
    {
	D( cout << "ClientHandler::~ClientHandler()" << endl; )
    }

    virtual Boolean handleOpen(Channel* channel)
    {
	D( cout << "ClientHandler::handleOpen()" << endl; )

	assert(channel->write(
	    LONG_MESSAGE, sizeof(LONG_MESSAGE)) == sizeof(LONG_MESSAGE));

	return true;
    }

    virtual Boolean handleInput(Channel* channel)
    {
	D( cout << "ClientHandler::handleInput()" << endl; )

	char buffer[1024];

	Sint32 size = channel->read(buffer, sizeof(buffer));

	if (size <= 0)
	    return false;

	D(
	for (Sint32 i = 0; i < size; i++)
	    ; // cout << buffer[i];

	)

	_received.append(buffer, size);

	if (_received.size() == sizeof(LONG_MESSAGE))
	{
	    assert(memcmp(
		_received.getData(), LONG_MESSAGE, sizeof(LONG_MESSAGE)) == 0);

	    cout << "+++++ passed all tests" << endl;
	    global_finished = true;
	    return false;
	}

	return true;
    }

    virtual Boolean handleOutput(Channel* channel)
    {
	D( cout << "ClientHandler::handleOutput()" << endl; )
	return true;
    }

    virtual void handleClose(Channel* channel)
    {
	D( cout << "ClientHandler::handleClose()" << endl; )
    }

private:

    Array<char> _received;
};

class ServerHandler : public ChannelHandler
{
public:

    ServerHandler()
    {
	D( cout << "ServerHandler::ServerHandler()" << endl; )
    }

    virtual ~ServerHandler()
    {
	D( cout << "ServerHandler::~ServerHandler()" << endl; )
    }

    virtual Boolean handleOpen(Channel* channel)
    {
	D( cout << "ServerHandler::handleOpen()" << endl; )
	return true;
    }

    virtual Boolean handleInput(Channel* channel)
    {
	D( cout << "ServerHandler::handleInput()" << endl; )

	char buffer[1024];

	Sint32 size = channel->read(buffer, sizeof(buffer));

	if (size <= 0)
	    return false;

	D( cout << "size=" << size << endl; )

	channel->write(buffer, size);

	return true;
    }

    virtual Boolean handleOutput(Channel* channel)
    {
	D( cout << "ServerHandler::handleOutput()" << endl; )
	return true;
    }

    virtual void handleClose(Channel* channel)
    {
	D( cout << "ServerHandler::handleClose()" << endl; ) 
    }
};

int main(int argc, char** argv)
{
    // -- Create common selector:

    Selector* selector = new Selector;

    // -- Create server side objects:

    ChannelHandlerFactory* serverFactory 
	= new DefaultChannelHandlerFactory<ServerHandler>;

    TCPChannelAcceptor acceptor(serverFactory, selector);
    assert(acceptor.bind("7070"));

    // -- Create client side objects:

    ChannelHandlerFactory* clientFactory 
	= new DefaultChannelHandlerFactory<ClientHandler>;

    TCPChannelConnector connector(clientFactory, selector);

    Channel* channel = connector.connect("localhost:7070");
    assert(channel);

    // -- Run the main loop:

    for (;;)
    {
	selector->select(5000);

	if (global_finished)
	    exit(0);
    }

    return 0;
}
