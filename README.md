# Trivia Game Backend

This is a trivia game developed by Shahar Yogev and Yuval Bar. The game presents a series of trivia questions to the
player, keeps score, and provides an enjoyable trivia experience.

## Demo
### [Here is a link](https://www.youtube.com/watch?v=NjpXF6E9N5U&feature=youtu.be) to the YouTube video showcasing our trivia game.

## Bonuses

1. Email Verification
2. Password Email Recovery
3. Regex User Data Validation + date validation
4. Dynamic Questions Fetching whenever more questions are needed
5. Update User Data Request
6. Player Avatar Colors Support
7. Cross Platform Support
8. CMake build system
9. Vcpkg package manager and manifest file
10. SQLITE_ORM to manage the database
11. Pretty Logging using the fmt library
12. CI/CD to release executables to multiple platforms including:
    - windows (exe and msix installers)
    - linux (deb, rpm and AppImage)
    - android (apk)
13. Music sound in game screen
14. Config file (`config.json` to set ip and port, the default is `0.0.0.0, 8826`)

## Getting Started

These instructions will help you get a copy of the project up and running on your local machine for development and
testing purposes.

### Prerequisites

- C++ compiler (supporting C++17 or later)
- CMake (version 3.27 or later)
- vcpkg (for managing C++ libraries)

### Building

#### note that when building on a Windows machine you should use the MinGW toolchain instead of visual-studio.

1. Clone the repository `git clone https://gitlab.com/DovIchilov/Trivia_yuval_bar_shahar_yogev_2024`
2. Navigate to `Trivia_yuval_bar_shahar_yogev_2024/trivia_backend`
3. Install all vcpkg dependencies (all packages are listed on `vcpkg.json` manifest file)
4. Create build directory `mkdir build` and navigate to it `cd build`
5. Configure CMAKE toolchain file, `cmake .. -DCMAKE_TOOLCHAIN_FILE=/path/to/vcpkg/scripts/buildsystems/vcpkg.cmake`,
   Replace `[path/to/vcpkg]` with the actual path to your vcpkg installation.
6. Build the project `cmake --build .`
7. Run the server `./trivia_backend`

#### If you are using Clion IDE, you can skip steps 3 and onwards, and use the builtin vcpkg integration and then run the server directly from clion.

### Running

#### when running note that you can create a `config.json` to override the ip or the port that the server will be working on

### Known Bugs

#### There aren't any known bugs, but there are some things that are good to know:

1. when a user submits answer the server will respond at the end of the question, meaning if he submits then suddenly
   closes the socket, he will be logged out only when trying to receive the next request (at the end of the current
   question).
2. when a user leaves the game before ending he will get punished (minus points) and his new score will be immediately
   submitted,
   and the other players score (the regular players which stayed in the game) will be submitted to the db when one of
   them leaves.
3. the emails are working with an external api called `mailjet`, we included the apikey directly in the code, although
   we
   know it's a bad practice, we did it just for simplicity, and if the project were being made for the public, we would
   have just use environment variables and load them at run time.
4. the `mailjet` api does not always succeed to send the mails, and sometimes it retries after a few minutes, so be
   patient with the emails and know that it's not perfect and emails can get lost, and you can always click on `resend`.

## Authors

- Shahar Yogev
- Yuval Bar
