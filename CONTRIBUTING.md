# Guidelines for submitting bugs:
All non security bugs should be filed on the Issues tracker:
https://github.com/flihp/tpm2-uefi/issues

See SECURITY.md for details on security bugs.

# Guideline for submitting changes:
All changes should be introduced via github pull requests. For pull requests
opened by non-maintainers, any maintainer may review and merge that pull
request.

## Patch requirements
* All tests must pass on Travis CI for the merge to occur.
* No patch may introduce superfluous changes or whitespace errors.
* All commits should adhere to the git commit message guidelines described
here: https://chris.beams.io/posts/git-commit/ with the following exceptions.
 * We allow commit subject lines up to 80 characters.
 * Commit subject lines should be prefixed with a string identifying the
effected subsystem. If the change is spread over a number of
subsystems then the prefix may be omitted.
* All contributions must adhere to the Developers Certificate of Origin. The
full text of the DCO is here: https://developercertificate.org/. Contributors
must add a 'Signed-off-by' line to their commits. This indicates the
submitters acceptance of the DCO.

## Guideline for merging changes
Changes must be merged with the "rebase" option on github to avoid merge commits.
This provides for a clear linear history.
