# Keero Tracks Module Firmware

The `tracks` module is the current public mobility firmware branch of Keero Bot.

It is the clearest open-source proof that the platform is already moving from concept into real hardware behavior.

## What It Does

The module currently focuses on the embedded basics needed for a compact tracked mobility unit:

- dual-motor style movement control
- WS2812 lighting modes
- battery monitoring
- UART command handling for external control and testing

## Hardware Direction

This mobility module is inspired by the ESP-SparkBot project and keeps visible credit to that original direction.

The Keero adaptation updates the module around `ESP32-C3-WROOM-M2`, which makes it a better fit for the current Keero toolchain and lets it run cleanly with Arduino and PlatformIO.

## Development Stack

The current public setup uses:

- PlatformIO
- Arduino framework
- `esp32-c3-devkitc-02` as the active board target in development

Main configuration lives in [platformio.ini](./platformio.ini).

## Reference Parts

The current public mobility build is associated with these practical reference parts:

- tracked chassis reference:
  [AliExpress link](https://www.aliexpress.com/item/1005011612359536.html?spm=a2g0o.order_list.order_list_main.52.1a141802qpneZl)
- 4-pin pogo connector:
  [AliExpress link](https://www.aliexpress.com/item/1005010390646641.html?spm=a2g0o.order_list.order_list_main.70.1a141802qpneZl)
- 3V DC motors, quantity `2`:
  [AliExpress link](https://www.aliexpress.com/item/1005005699205905.html?spm=a2g0o.productlist.0.0.75e3MQOnMQOn9t)

These are documented as reference parts used around the current module direction, not as a locked mass-production BOM.

## Public Command Direction

The current firmware already exposes a simple serial command model for bring-up and testing:

- `x... y...` style motion commands
- `w...` for lighting mode changes
- `c...` for motion tuning
- `d...` for test behavior

That makes the module easy to demo and iterate while the broader Keero platform firmware is still being built out.

## Why It Matters

This module is important because it turns Keero from a stationary electronics story into a mobility-capable platform story.

It demonstrates:

- real firmware ownership
- practical embedded integration
- a path toward embodied AI experiments
