# Brother embroidery card experiments
I wanted to try and get an old Brother NV500D working with own digitised PES files.

These machines have only got a proprietary memory card slot, which takes brother design cards.

Rewritable cards were available from many manufacturers, including Brother, but they've since mostly fizzled away as the newer machines have USB on board.

This repository is my documentation for a custom design card PCB and software to write images to these cards.

## The PCB
The brother design cards use parallel memory ICs, with 19 address pins and 8 data pins.
My initial experimental PCB is based on the [esp32 PCB done by AeroX2](https://github.com/AeroX2/brother-cart-emulator), but uses a Teensy 3.5 because that's what I had lying around - lots of 5V tolerant GPIO so I didn't need to order shift register/buffer ICs.

### Teensy firmware
The teensy firmware is pretty clunky right now. It's based on AeroX2's nicer esp32 firmware.

## Generating images and programming the flash

This pretty clunky right now because I'm just trying to get it to work and hopefully get a custom program written to create the binary image to be loaded onto the flash.
First, use [AeroX2's image-dumper'](https://github.com/AeroX2/brother-cart-emulator/tree/main/image-dumper) or my modified version of [mlueft's vikant emulator](https://github.com/mlueft/emcr/tree/master/emulators/vikantUltimateBox) to create an `image.bin` from a `PEC` or `PES` file.
Place `image.bin` on a micro SD card (make sure it is named image.bin).

Plug the Teensy into the memory card PCB, and the SD card into the Teensy and issue a "W" command via a serial monitor.
You can use "V" to verify that the file written to the flash chip matches the `image.bin` on the SD card.

**NOTE: DO NOT HAVE THE MEMORY CARD PLUGGED INTO THE MACHINE POWERED ON AT THE SAME TIME AS HAVING THE TEENSY PLUGGED IN AND CONNECTED VIA USB. THIS BOARD CURRENTLY HAS 0 SAFETY GUARANTEES AND YOU WILL POSSIBLY EXPLODE THE TEENSY**

### Using the Vikant software
Unfortunately I haven't got this part working on linux yet, so I begrudgingly boot into Windows until our own software to write to the cards is created.

1. Download and install the old [Vikant Ultimate Box serial port](http://vikant-emb.com/downloads1/UltimateExplorerInstallation.exe) software (the previous link is the direct download. [Go the bottom of this page if it doesn't work.](https://vikant-emb.com/downloads)).
2. Download and install [`com0com`](https://com0com.sourceforge.net/).
3. Edit the `main.py` in the `card_emulator` directory to use one of the COM ports created with com0com (use device manager to figure it out). 
4. Assuming you have python installed, start an administrator command prompt and `cd` to the folder containing `main.py`.
5. Run python `main.py`.
6. Start the UltimateExplorer (Brother) software. It should detect a card on a COM port and let you write files.
7. Copy `image.bin` from the `card_emulator` directory to the microSD card and follow the instructions above to get it written to the memory card.

#### Dirty hacks
Eventually, there will be a direct pipeline from PES file -> python -> directly written to the flash memory.
At the moment, I have just edited mlueft's code to create a 512K data file to match our flash chip.
Vikant still thinks it's a 1024K chip, so if you write more than 512K worth of data, it will cause issues.

## Experiments
I am trying to figure out the format of the binary data written to the brother embroidery card.
[This EEVblog forum post](https://www.eevblog.com/forum/reviews/brother-(possibly-also-bernina)-embroidery-machine-memory-cards/) was very helpful and is what initially introduced me to AeroX2's stuff. There is a wealth of information in that thread.

The `experiments.ipynb` jupyter notebook contains an interactive plotting routine that will go through the `image.bin` file and plot the data as a bitmap. I did this to figure out where and what thumbnails are stored on the card.

The stitch data is stored at the very end of the card, with thumbnails for the embroidery patterns stored before that.

There are also many random thumbnails in lots of languages for UI elements. It appears that the embroidery card also stores the UI for anything inside the "design card menu" from the LCD screen of the machine - probably thanks to the limited storage these old machines had.

Many of the elements of a standard PEC file are present in this file, but the order is different.

The main thing we need to figure out is where the values which point to the start of the stitch data and thumbnails for each PES file written to the card are stored.

# Reverse engineering the file structure
See [notes.org](notes.org)
