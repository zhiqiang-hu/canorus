/** @file note.h
 * 
 * Copyright (c) 2006, Matevž Jekovec, Canorus development team
 * All Rights Reserved. See AUTHORS for a complete list of authors.
 * 
 * Licensed under the GNU GENERAL PUBLIC LICENSE. See COPYING for details.
 */

#ifndef NOTE_H_
#define NOTE_H_

#include "core/muselement.h"
#include "core/playable.h"

class CAVoice;

class CANote : public CAPlayable {
	public:
		enum CANoteLength {
			None = -1,
			Breve = 0,
			Whole = 1,
			Half = 2,
			Quarter = 4,
			Eighth = 8,
			Sixteenth = 16,
			ThirtySecond = 32,
			SixtyFourth = 64,
			HundredTwentyEighth = 128
		};
		
		CANote(CANoteLength length, CAVoice *voice, int pitch, signed char accs, int timeStart, int timeLength=0);
		CANote *clone();
		
		CANoteLength noteLength() { return _noteLength; }
		int pitch() { return _pitch; }
		const QString pitchML();	///Compose the note pitch name for the CanorusML format
		const QString lengthML();	///Compose the note length for the CanorusML format
		void setPitch(int pitch) { _pitch = pitch; calculateNotePosition(); }
		int notePosition() { return _notePosition; }

		static const QString generateNoteName(int pitch);

	private:
		/**
		 * Depending on the clef, calculate note position in the staff.
		 */
		void calculateNotePosition();
	
		CANoteLength _noteLength;
		int _pitch;	///note pitch in logical units. 0 = C,, , 1 = Sub-Contra D,, , 56 = c''''' etc.
		int _accs;	///note accidentals. 0 = neutral, 1 = sharp, -1 = flat etc.
		int _notePosition;	///note location in the staff. 0 first line, 1 first space, -2 first ledger line below the staff
};
#endif /*NOTE_H_*/

