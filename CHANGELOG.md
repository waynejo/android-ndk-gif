## 0.3.1 (2016-07-25)

Bugfixes:
  -  Fix issue #3. Remove unexpected data from result file when encode gif.

## 0.3.0 (2016-07-04)
  - Add some encoding options.
    - ENCODING_TYPE_SIMPLE_FAST 
      - use low memory and encode fast. But low quality.
    - ENCODING_TYPE_NORMAL_LOW_MEMORY 
      - use lower memory than ENCODING_TYPE_STABLE_HIGH_MEMORY. and image changing dynamic.
    - ENCODING_TYPE_STABLE_HIGH_MEMORY 
      - slowest and use high memory. But high quality and stable image sequence.

## 0.2.5 (2016-06-04)

Bugfixes:
  -  Fix a crash when encoding with little color bitmap.

## 0.2.4 (2016-05-21)

Bugfixes:
  -  Fix a bug that twitter can't load a GIF file what encoded using this library.

## 0.2.3 (2016-05-15)

Features:
  - Improve to prevent blinking color when encoding.

## 0.2.2 (2016-05-03)

Bugfixes:
  -  Fix a bug that right most pixels didn't rendered correctly.
  -  Fix a crash.
  -  Fix Problem that some android devices's gallery can't display correctly.

## 0.2.1 (2015-10-12)

Bugfixes:

  - Not enocded collectly.

Features:

  - Ignore same image regions when write gif.
  - Improve little encoding speed.

## 0.2.0 (2015-09-22)

Features:

  - Encoding GIF


## 0.1.0 (2015-08-14)

Features:

  - Decoding GIF

