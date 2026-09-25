#!/usr/bin/env bash
# Report GitHub release asset download counts for one or more tags.
#
# Usage:
#   ./gh_release_downloads.sh <owner>/<repo> [tag ...]
#
# Examples:
#   ./gh_release_downloads.sh dvorka/mindforger 2.2.0 2.1.0
#   ./gh_release_downloads.sh dvorka/mindforger          # all releases
#
# Notes:
#   - Uses the public GitHub REST API, no auth required.
#   - Unauthenticated requests are rate-limited to 60/hour. To raise the
#     limit, export GITHUB_TOKEN and the script will send it as a Bearer
#     token automatically.

set -euo pipefail

repo="${1:?usage: $0 <owner>/<repo> [tag ...]}"
shift || true
tags=("$@")

auth_header=()
if [[ -n "${GITHUB_TOKEN:-}" ]]; then
  auth_header=(-H "Authorization: Bearer ${GITHUB_TOKEN}")
fi

fetch_release() {
  local tag="$1"
  curl -sf "${auth_header[@]}" \
    "https://api.github.com/repos/${repo}/releases/tags/${tag}"
}

report_release() {
  local json="$1"
  local tag total
  tag=$(jq -r '.tag_name' <<<"$json")
  echo "=== ${tag} ==="
  jq -r '.assets[] | "\(.download_count)\t\(.name)"' <<<"$json" | sort -rn
  total=$(jq '[.assets[].download_count] | add // 0' <<<"$json")
  echo "--- total: ${total} ---"
  echo
}

if [[ ${#tags[@]} -eq 0 ]]; then
  # no tags given: report every release
  curl -sf "${auth_header[@]}" "https://api.github.com/repos/${repo}/releases" \
    | jq -c '.[]' \
    | while IFS= read -r release_json; do
        report_release "$release_json"
      done
else
  for tag in "${tags[@]}"; do
    report_release "$(fetch_release "$tag")"
  done
fi
