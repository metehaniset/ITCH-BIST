# BIST(Borsa Istanbul) ITCH Book Constructor
> Given the BIST ITCH data feed, reconstruct the full depth order book and related messages.

I changed the code according to https://www.borsaistanbul.com/files/bistech-itch-protocol-specification.pdf


## Usage example

To reconstruct the book compile and use the binary in bin folder:

```

bin/BookConstructor /home/hft/data/binary/20200804/TR-1.VRD /home/hft/data/book/ /home/hft/data/messages/ 10 GARAN.E

```





# Folder Structure

```
ITCH
│
└───bin     (c++ executable)
│
└───build   (dependency and object files)
│
└───doc
│   │
│   └───latex       (LaTex-pdf documentation created with Doxygen)
│
└───data
│   │
│   └───binary      (ITCH binary gzip compressed files from NASDAQ)
│   │
│   └───book        (output of book csv)
│   │
│   └───messages    (output of messages csv)
│   │
│   └───pk          (pickle object files)
│
└───gtests    (.cpp for tests)
│
└───images    (images for README.md Markdown)
│
└───include   (.hpp for main program)
│
└───examples  (python implementation of some application example)
│
└───src       (.cpp for main program)

```
