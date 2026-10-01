# Widgets in KalaGraphics

This document is for describing the goals of widgets in KalaGraphics.

All widgets originate from resources. The primary, and probably the only resource, for a widget is a Mesh.

Widgets split into two categories - primitive and composite widgets.

## Primitive Widgets

Primitive widgets are composed from resources. Primitive widgets will be things like Text, ClipArea, Scrollbar, Button.

## Composite Widgets

Composite widgets are complex widgets consisting of multiple primitive widgets.

Expected composite widgets will be things like Console, Dropdown (probably multiple types), InputField and more.