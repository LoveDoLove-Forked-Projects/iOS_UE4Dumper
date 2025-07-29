# iOS Unreal Engine Dumper / UE Dumper

MobileSubstrate Tweak to dump Unreal Engine games on iOS.

The dumper is based on [UE4Dumper-4.25](https://github.com/guttir14/UnrealDumper-4.25)
project.

## Features

* Supports ARM64 & ARM64e
* CodeSign friendly, you can use this as a jailed tweak
* Dumps UE offsets classes, structs, enums and functions
* Generates function names json script to use with IDA & Ghidra
* Pattern scanning to find the GUObjectArray, GNames and FNamePoolData addresses automatically
* Find GWorld and GEngine in '__DATA'
* Transfer dump files via AirDrop/[LocalSend](https://github.com/localsend/localsend)

## Currently Supported Games

* Ark Ultimate
* Arena Breakout
* Black Clover M
* Delta Force
* Dislyte
* Farlight 84
* Injustice 2
* Mortal Kombat
* Odin Valhalla Rising
* Real Boxing 2
* Rooftops Parkour
* The Baby In Yellow
* Torchlight: Infinite
* Wuthering Waves
* Blade Soul Revolution
* Lineage 2 Revolution
* eFootball (PES)
* King Arthur Legends Rise
* Night Crows
* Century Age Of Ashes
* Case 2 Animatronics
* Hello Neighbor
* Hello Neighbor Nicky's Diaries
* Special Forces Group 2
- Final Fantasy 14

## Usage

Install the debian package on your device.
Open one of the supported games and wait for the message pop-up to appear, It will say that the dumping will begin soon.
Wait for the dumper to complete the process.
Another pop-up will appear showing the dump result and the dump files location.
After this a third pop-up will appear showing you the optional function to share/transfer the dump files.

## Output-Files

### AIOHeader.hpp

* An all-in-one dump file header

### Offsets.hpp

* Header containing UE Offsets

### Logs.txt

* Log file containing dump process logs

### Objects.txt

* ObjObjects dump

### script.json

* If you are familiar with Il2cppDumper script.json, this is similar
* It contains a json array of function names and addresses

## How to transfer the dump from the device to the pc

You can use AirDrop or [LocalSend](https://github.com/localsend/localsend) to transfer dump to any device within local network.

## Adding or updating a game in the dumper

Follow the prototype in [GameProfiles](Tweak/src/UE/UEGameProfiles)<br/>
You can also use the provided patterns to find GUObjectArray, GNames or NamePoolData.

## Building

```bash
git clone --recursive https://github.com/MJx0/iOS_UEDumper
cd iOS_UEDumper/Tweak
make clean package
```

## Credits & Thanks

* [UE4Dumper-4.25](https://github.com/guttir14/UnrealDumper-4.25)
* [Il2cppDumper](https://github.com/Perfare/Il2CppDumper)
* [Dumper-7](https://github.com/Encryqed/Dumper-7)
* [UEDumper](https://github.com/Spuckwaffel/UEDumper)
* @Katzi for testing and writing this README for me XD
