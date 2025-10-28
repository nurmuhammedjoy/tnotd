<div align="center">

# tnotd

### Termux Notification Daemon for X11

<p>
  <img src="https://img.shields.io/badge/language-C-blue.svg" alt="Language">
  <img src="https://img.shields.io/badge/license-GPL--3.0-green.svg" alt="License">
  <img src="https://img.shields.io/github/last-commit/nurmuhammedjoy/tnotd" alt="Last Commit">
  <img src="https://img.shields.io/github/stars/nurmuhammedjoy/tnotd" alt="Stars">
</p>

<p>A lightweight Termux daemon written in C that captures Android notifications via the Termux:API service and displays them in X11-based environments using GTK</p>

</div>


## 📋 Table of Contents

- [Overview](#-overview)
- [Features](#-features)
- [Requirements](#-requirements)
- [Contributing](#-contributing)
- [License](#-license)



## 🎯 Overview

**tnotd** (Termux Notification Daemon) bridges Android notifications to X11 desktop environments running on Termux. It listens for Android system notifications through the Termux:API





### System Dependencies

- **Termux** app (from F-Droid or GitHub)
- **Termux:API** app (from F-Droid or GitHub)
- **termux-api** With proper permissions granted

### Build Dependencies

```bash
pkg install git clang gtk3 libnotify termux-api xorgproto
```
### How to Build and Install

```bash
git clone https://github.com/nurmuhammedjoy/tnotd.git

cd tnotd
make
make install
make clean

```
### how to use 

if you want to just display any notification then 

```bash

tnotd "title" "description" image.file

```

and if you want to run notification daemon

```bash

daemon

```

also you can configure what you want to display 

[daemon.c]
```bash
// configurations
AppConfig app_configs[] = {
    {"com.discord", "discord", "tnotd '{TITLE}' '{CONTENT}' images/discord.png"},
    {"com.spotify.music", "spotify", "tnotd 'Artist: {CONTENT}' 'Song: {TITLE}' images/spotify.png"},
    // add more apps here if needed

};
```
>> note: most of the initial like 30-40% code written by using ai so it has some work 

 
[preview.png]
[preview1.png]

## 🤝 Contributing

Contributions are welcome! Feel free to:

- 🐛 Report bugs
- 💡 Suggest features
- 🔧 Submit pull requests

<div align="center">

### How to Contribute

</div>

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

---

## 📄 License

<div align="center">

This project is licensed under the **GNU General Public License v3.0**

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)

See [LICENSE](LICENSE) for more information.

</div>

---

<div align="center">

### 🌟 Star this repository if you find it useful!

Made with ❤️ by [nurmuhammedjoy](https://github.com/nurmuhammedjoy)

<p>
  <a href="https://github.com/nurmuhammedjoy/tnotd/issues">Report Bug</a>
  ·
  <a href="https://github.com/nurmuhammedjoy/tnotd/issues">Request Feature</a>
</p>

</div>
