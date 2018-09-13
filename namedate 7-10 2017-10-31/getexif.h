/*
 * getexif.h
 *
 *  Created on: 13.05.2017
 *      Author: James
 */

#ifndef GETEXIF_H_
#define GETEXIF_H_

typedef struct tagExifDates {
	time_t tv_mod;
	time_t tv_org;
	time_t tv_dig;
} rtnExifDates;

int ExifDates(char *szIn, rtnExifDates *pDT);

#endif /* GETEXIF_H_ */
