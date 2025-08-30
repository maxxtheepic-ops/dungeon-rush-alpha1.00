#ifndef MENU_BASE_H
#define MENU_BASE_H

#include "../input/Input.h"
#include "../graphics/Display.h"

enum class MenuResult {
    NONE,           // No selection made yet
    SELECTED,       // User made a selection
    CANCELLED,      // User cancelled/backed out
    EXIT            // Exit to previous menu/state
};

class MenuBase {
protected:
    int selectedOption;
    int maxOptions;
    bool isActive;
    int selectionMade;  // Stores the actual selection value
    
    Display* display;
    Input* input;
    
    // Helper methods for common menu operations
    void moveSelectionUp();
    void moveSelectionDown();
    void wrapSelection(); // Handles wrapping around menu options
    
public:
    MenuBase(Display* disp, Input* inp, int numOptions);
    virtual ~MenuBase() = default;
    
    // Core menu interface - must be implemented by each menu
    virtual void render() = 0;
    virtual MenuResult handleInput() = 0;
    
    // Common functionality
    virtual void activate();
    void deactivate();
    bool getIsActive() const { return isActive; }
    
    // Get the current selection (0-based index)
    int getCurrentSelection() const { return selectedOption; }
    
    // Get the result of the last selection made
    int getSelectionResult() const { return selectionMade; }
    
    // Reset menu to initial state
    virtual void reset();
};

#endif