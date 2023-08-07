# Open both files
with open('ToCheck.txt', 'r') as to_check_file, open('Valids.txt', 'r') as valids_file, open('Invalids.txt', 'w') as invalids_file:
    # Read all valid lines into a set (for faster lookups)
    valids = set(valid_line.strip() for valid_line in valids_file)
    
    # For each line in to_check_file
    for line in to_check_file:
        # Remove the newline character
        line = line.strip()

        # Check if the exact text exists in the Valids.txt
        found = False
        for v in valids:
            if line in v:
                found = True
                break    
        
        if(not found):
            # Write the line to Invalids.txt
            invalids_file.write(line + '\n')

