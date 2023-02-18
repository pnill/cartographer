## Project Cartographer
This code is intended to simulate XLive libraries over System Link for Halo 2 Vista. It also adds a wide variety of extra features into the game itself.
Official Website: https://cartographer.online/

## Contributing
Pull requests are the best way to submit any additions or fixes. Small changes or tweaks that don't affect functionality (like appearance of something for example) should be communicated to the developers as feedback since we may not agree with such a change/choice. If you intend to work (and are confident you won't just bail on the project) on something that requires online play, you will need to join the #dev_preview program in order to be able to run unofficial builds by logging in to a test account. Ask one of the developers about joining it. You however are expected to already be experienced in C/C++. Experience in reverse engineering binaries will help get you far.

## Pull Request Guidelines

	* In depth explanation why this PR is important to the project and what it accomplishes

	* Don't rewrite the project, if you want to make a large project/build level change come to the discord, and talk it over with at least 2 developers

	* Provide proof of testing:

		- If you made a minor change rigourous testing may not be required
		- If you modify the graphics
		
			- 8 players minimum
			- 10 game minimum
				- Players should not all be in same geographic area			
			- Different machines with different graphics cards
			
		- If you modify the gameplay
			- 16 players minimum
			- 10 game minimum
				- Players should not all be in same geographic area		
				
		- Provide date/times of test with discord usernames involved with small explanation of each test
		- Post your dll in the discords channel "dev_preview"
			
	* Don't add 3rd party libraries without at least 2 developers approval

	* Don't redo features without at least 2 developers approval

	* Explain your offsets

		- i.e., you know some static offset in memory controls the warthog horn, so explain how you determined this and explain why changing this value works
		- Don't just tweak 100 different values expecting the code reviewer to know if your values are correct or not.
		
	* Explain your hooks

		- Don't just hook a bunch of methods without explaining why these hooks are the right way to do it.
		
	* Provide screenshots or video of your change in effect (mainly for graphics/gameplay changes)

	* Vague Coding Standards:

		- WARNING: The current code base does not really live up these standards, but future code changes should stride to meet them.
		
		- http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines
		
		- All methods should first character should be lowercase and the rest should be camelcase
		- Functions larger than 50 lines should be discouraged
		- Function names should reflect exactly what the function is supposed to do
		- Functions should not have multiple purposes (i.e., don't make a function mutate your data, then send it, then turn off the warthog horn, etc)
		- Be mindful of spawning new threads in certain hooks (i.e., don't spawn a thread every time the network membership function is executed)

## Discord - Online Social Hangout
All Developers/Staff/Associates of this project hang out and work on this project via its official Discord server.
Invitation link: https://cartographer.online/discord

### Development Build Status
[![Build status](https://ci.appveyor.com/api/projects/status/jduukxfn4s14nvgq/branch/development-new?svg=true)](https://ci.appveyor.com/project/pnill/cartographer/branch/development-new)
