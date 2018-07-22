# zhaDataAcquisition
Data acquisition method for TAMIZH OCR

We have used opencv-C++ to acquire Data from a scan of tabled sheet and splitting the characters into individual images.
This is a part of a OCR project for TAMIZH handwritten script.
It could be very well used for data acquisition of any language script.

Instructions:
Note: Make sure CMAKE and OPENCV is installed
- Run "make" from project root folder to generate binary executable file.
- After that you can choose to extract individual images
by running "./DataAcquisition <raw image source file> <destination folder path>
- If you want to extract all raw images simply run 
"./extract.sh" from the project root folder

