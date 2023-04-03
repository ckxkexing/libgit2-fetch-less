//
// Created by 15857 on 2022/11/24.
//
#include<iostream>

extern "C" {
    #include "git2.h"
}

using namespace std;

git_repository *repo = NULL;

int main(int argc, char **argv){
	git_libgit2_init();
	// const char *url = "https://github.com/lu-zero/cargo-c.git";
    // const char *path = "../tmp_310";
    if (argc != 3) {
        puts("input error");
        return 0;
    }
    const char *path = argv[1];
    const char *url = argv[2];
	git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;

    // redirect refs db:

	int error = git_clone(&repo, url, path, NULL);

	if (error < 0) {
		const git_error *e = git_error_last();
		printf("Error %d/%d: %s\n", error, e->klass, e->message);
		exit(error);
	}
	git_libgit2_shutdown();
	return 0;
}
