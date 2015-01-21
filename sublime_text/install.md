# Installation of Sublime Text goodies

## Vintageous Toggler

Put this command in Packages/User/vintageous_toggler.py:

```python
import sublime
import sublime_plugin


class ToggleVintageous(sublime_plugin.WindowCommand):
    def run(self):
        setts = sublime.load_settings('Preferences.sublime-settings')
        ignored = setts.get('ignored_packages')

        if 'Vintageous' in ignored:
            ignored.remove('Vintageous')
        else:
            ignored.append('Vintageous')

        setts.set('ignored_packages', ignored)
        sublime.save_settings('Preferences.sublime-settings')
```

Then create Packages/User/Default.sublime-commands and add this:

```python
[
    { "caption": "Vintageous: Toggle", "command": "toggle_vintageous" }
]
```

You can now toggle Vintageous from package control using "Vintageous: Toggle".

## Save on focus lost

Place this line in your Preferences -> User:

```json
    "save_on_focus_lost": true,
```
Switching to another window will save all tabs.