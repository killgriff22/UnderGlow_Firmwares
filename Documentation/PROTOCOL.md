# SERIAL PROTOCOL
I wrote a "simple" serial protocol for the keypad and the firmware to agree on certain things.  
The protocol flow goes like so:  
* The LED controller, or the builtin serial bus sends a 3 byte request, structured:  
    * FA-FD - What kind of request is this? It can be:  
        * FA, A read request  
        * FB, A write request  
        * FC, A reset request  
        * FD, A format request  
    * 00-FF - Where to read from/write to, unused in a reset or format request.  
    * 00-FF - A value to write, exclusively used in a write request  
* Then, the keypad always responds with the value at the address specified, or the message defined inside the program in the case of a reset/format request.  
The keypad and the controller will never use more than 256b of EEPROM, as that is as far as the protocol can reach.  
The data that needs to be stored and requested is, and where it is stored:  
* the color, in 3 different bytes, 0x00/0x01/0x02  
* the pattern to use, 0x03   
* the brightness of the leds, 0x04  
* the speed, 0x05  
* the increment, 0x06, unused in the led controller  
* the direction, 0x07, unused in both  
* the message to send after a format or reset request, 0xFC  
The controller has the ability to stand on it's own by timing out the read requests that it perfoms.  