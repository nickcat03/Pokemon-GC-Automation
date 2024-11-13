# Pokemon-GC-Automation

These Arduino scripts focus on automating tedious tasks in Generation 3 Pokemon games, including auto leveling, Luxury Ball farming, and more in the future. They are dependent on the [Nicohood](https://github.com/NicoHood/Nintendo) library.

# Hardware
- Arduino Mega 2560 (clones such as the Elegoo Uno R3 also work)
- Logic Controller 5v - 3.3v Bidirectional
- 1k resistor
- Gamecube Controller Cable ("male" end needed only)
- Soldering iron and basic soldering experience required

Building the Arduino follows the same process as [this guide](https://docs.google.com/document/d/1ndV4E_w2ThAv9LpbjEzRDWwMj0uBWB9YqbvkHV35X5w/edit?tab=t.0), without the need of wiring external buttons. This script runs on its own the moment it is plugged into the Gamecube and does not require any other controller to interface with it.

You can also follow this image for reference, ignoring the connections marked in purple as they are not needed.
![image](https://github.com/user-attachments/assets/ff21f7c1-01c5-450c-ae97-df33a9cc20b8)

# Running the Scripts
Each script runs automatically once it is plugged into the Gamecube console. Instructions for preparing individual scripts are located in the README in each folder.
