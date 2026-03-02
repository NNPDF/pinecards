#!/bin/bash

TEMPLATE_DIR="TEMPLATE"

for dir in */; do
    dir=${dir%/}

    if [ "$dir" = "$TEMPLATE_DIR" ]; then
        continue
    fi

    if [ -d "$dir" ]; then
        echo "Processing $dir"

        cp "$TEMPLATE_DIR/output.txt"   "$dir/"
        cp "$TEMPLATE_DIR/launch.txt"   "$dir/"
        cp "$TEMPLATE_DIR/run.sh"       "$dir/"
        cp "$TEMPLATE_DIR/setscales.f"  "$dir/"

        # Replace TEMPLATE with dataset name
        sed -i "s/TEMPLATE/$dir/g" "$dir/output.txt"
        sed -i "s/TEMPLATE/$dir/g" "$dir/launch.txt"
        sed -i "s/TEMPLATE/$dir/g" "$dir/run.sh"
        sed -i "s/TEMPLATE/$dir/g" "$dir/setscales.f"

        # Set ebeam values depending on energy in directory name
        ebeam=""
        if [[ "$dir" == *"13P6TEV"* ]]; then
            ebeam="6800"
        elif [[ "$dir" == *"13TEV"* ]]; then
            ebeam="6500"
        elif [[ "$dir" == *"8TEV"* ]]; then
            ebeam="4000"
        elif [[ "$dir" == *"7TEV"* ]]; then
            ebeam="3500"
        elif [[ "$dir" == *"5TEV"* ]]; then
            ebeam="2510"
        fi

        if [ -n "$ebeam" ]; then
            # Replace whole line, preserving the "set ebeam1/2" key.
            sed -i -E "s/^([[:space:]]*set[[:space:]]+ebeam1)[[:space:]]+.*/\1 $ebeam/" "$dir/launch.txt"
            sed -i -E "s/^([[:space:]]*set[[:space:]]+ebeam2)[[:space:]]+.*/\1 $ebeam/" "$dir/launch.txt"
        fi
    fi
done

echo "Done."
