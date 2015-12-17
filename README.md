#Hitchhiker Laboratories Patterner
##About
The Patterner is a modulation generator that is meant to replace an expression pedal for use with guitar pedals that have an expression input.

Instead of the single potentiometer present on a regular expression pedal, the Patterner has four slide potentiometers labelled 00, 01, 10 and 11. The two digit numeric code corresponds to the state (off=0, on=1) of the two left-most buttons (orange and purple). Pressing the buttons select with potentiometer modulates the guitar pedal connected to the Patterner's TRS output. The sliders' LED lights up to indicate currently selected slider.

The Patterner also allows the recording and playback of sequences played on these two buttons. When the status LED above the right-most button is off, pressing the right-most button (black) once will arm the recording (LED turns red). The sequence will begin recording with the next press of the purple or orange buttons and will end with the next press of the black button. Playback of the sequence will start immediately after this second press of the black button (LED turns green). Pressing the black button during playback will pause the playback of the sequence (LED turns black) and pressing it again will restart the sequence. A long press of the black button at any time will clear the recorded sequence (LED turns off).

While a sequence is playing the Patterner's Speed control lets the player vary the playback speed of the recorded sequence.

##Implementation
###Code
The Patterner is implemented with Arduino and requires the metro library. Buttons are not debounced in software (see debouncing below).

Code: patterner.ino

###Hardware
Arduino Pro Mini is the ideal platform for small guitar pedals.

Arcade buttons are ideal for quick pattern playing with either hands or feet.

I used these sliders: (45mm version): http://www.mouser.com/ds/2/54/tl-777483.pdf

Suggested enclosure: http://www.hammondmfg.com/pdf/1590XX.pdf

Enclosure template: enclosure.pdf

Example perfboard layout: pefboard.pdf

Schematic: schematic.pdf

###Debouncing
Schematic shows debouncer implemented by Murat Uzam with PIC16F628: http://www.electronicsworld.co.uk/

Feel free to use whatever debouncing strategy you like best, but be aware that the arduino code as written does not debounce the buttons at all.

##Licence
Patterner by Sylvain Poitras is licensed under the GNU General Public License v.3: https://www.gnu.org/copyleft/gpl.html

##Thanks
Looping engine code adapted from Glenn Mossy's sequence recorder (Creative Commons CC-BY-SA) at http://littlebits.cc/projects/sequence-recorder

Iterrupt setup code from: http://playground.arduino.cc/Main/PinChangeInterrupt
