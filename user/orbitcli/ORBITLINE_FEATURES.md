# OrbitLine Features

OrbitLine is the user-friendly command editor for OrbitCLI.

## Editing

- Left and right arrows move the cursor
- Up and down arrows browse history
- Home and End move to line boundaries
- Backspace deletes before the cursor
- Delete removes the character under the cursor
- Ctrl+C cancels the command
- Ctrl+L clears the display
- Ctrl+U clears the entire input line
- Ctrl+W deletes the previous word

## Suggestions

When the user presses Tab:

1. Search the OrbitCLI command registry.
2. Search files in the current directory.
3. Complete an exact match.
4. Display multiple matches if necessary.
5. Add a space after a completed command.

Example:

    [UNIOS::ORBIT] pilot@ship:/ > dr<Tab>

becomes:

    drivers

## History

History should eventually be stored at:

    /home/<user>/.orbit_history

Do not write passwords, tokens, or private command arguments to history.

## Fuzzy query

Ctrl+R should search previous commands:

    (reverse-search): rescan

Press Enter to reuse the selected command.
