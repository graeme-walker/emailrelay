# feature/git-support

This branch has miscellaneous git-specific stuff.

The new GitHub Actions workflow file publishes a GitHub draft
release when pushing a "release/v*" branch, or a full release
when pushing a "v*" tag. The tarball is uploaded as an asset and
"doc/RELEASE.md" is used as the body text.

A 'make dist' hook is added to make sure that Windows batch files
have CRLF line-endings in the tarball. Git should normally
store these with LF line-endings in the repository but check
them out as CRLF on a Windows. A git pre-commit hook script
is included that can ensure that batch files are not checked
in to the respository with CRLF line endings when running on
Unix and this can be installed with 'make git-hooks'.
