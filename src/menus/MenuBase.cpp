#include "MenuBase.h"

MenuBase::MenuBase(Display* disp, Input* inp, int numOptions) {
    display = disp;
    input = inp;
    maxOptions = numOptions;
    selectedOption = 0;
    isActive = false;
    selectionMade = -1;
}

void MenuBase::activate() {
    isActive = true;
    selectedOption = 0;  // Reset to first option
    selectionMade = -1;  // Clear previous selection
}

void MenuBase::deactivate() {
    isActive = false;
}

void MenuBase::reset() {
    selectedOption = 0;
    selectionMade = -1;
    isActive = false;
}

void MenuBase::moveSelectionUp() {
    selectedOption--;
    wrapSelection();
}

void MenuBase::moveSelectionDown() {
    selectedOption++;
    wrapSelection();
}

void MenuBase::wrapSelection() {
    if (selectedOption < 0) {
        selectedOption = maxOptions - 1;  // Wrap to bottom
    } else if (selectedOption >= maxOptions) {
        selectedOption = 0;  // Wrap to top
    }
}