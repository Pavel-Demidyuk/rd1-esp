#!/bin/bash

# Get the current git tag
current_tag=$(git tag | sort -V | tail -1)

# Split the tag into its version parts
IFS='.' read -r -a version_parts <<< "$current_tag"

# Increment the patch version
((version_parts[1]++))

# Create the new tag
new_tag="${version_parts[0]}.${version_parts[1]}"

# Create a new git tag
git tag $new_tag

# Push the new tag to the remote repository
git push origin $new_tag