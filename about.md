# About

`fbpanel` is a lightweight, [`NetWM`] compliant desktop panel.
It works with any [`NetWM`] compliant window manager
(e.g. [`xfwm4`], [`sawfish`], [`openbox`], [`metacity`], [`KDE WM`]).

[`NetWM`]: https://www.freedesktop.org/wiki/Specifications/wm-spec/
[`xfwm4`]: https://xfce.org/
[`sawfish`]: https://sawfish.fandom.com
[`openbox`]: http://openbox.org
[`metacity`]: http://www.gnome.org/softwaremap/projects/metacity/
[`KDE WM`]: https://kde.org/

It provides

- variety of plugins
    - `taskbar`
    - `pager`
    - 'show desktop' button
    - `launchbar`
    - image viewer
    - text clock
    - digital clock
    - `tray` (notification area)
    - menu with support for `freedesktop.org` (as in `GNOME`/`KDE`) applications menu
    - space filler
    - cpu monitor
    - network monitor
    - memory monitor
    - battery monitor
    - volume regulator
    - general monitor - puts output of configured command in a panel
- ability to replace windows' icons
- all icons follow your current `GTK+` icon theme
- transparency support
- customizable size and screen position
- ability to run many instances each with its own configuration
- modest resource usage

It's written in C and uses the [`GTK+`] library only.

[`GTK+`]: https://www.gtk.org/

Latest version is [`7.0`](https://github.com/aanatoly/fbpanel).
