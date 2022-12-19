//
// Created by 15857 on 2022/11/24.
//
#include<iostream>

extern "C" {
	#include "git2.h"
}

using namespace std;

int main(){
	git_libgit2_init();
	const char *url = "https://github.com/lu-zero/cargo-c.git";
	const char *path = "../tmp";
	git_clone_options clone_opts = GIT_CLONE_OPTIONS_INIT;
	// clone_opts.fetch_opts.proxy_opts.type = GIT_PROXY_SPECIFIED;
	// clone_opts.fetch_opts.proxy_opts.url = "http://172.25.32.1:7890";
	git_repository *repo = NULL;
	int error = git_clone(&repo, url, path, NULL);
	if (error < 0) {
		const git_error *e = git_error_last();
		printf("Error %d/%d: %s\n", error, e->klass, e->message);
		exit(error);
	}
	git_libgit2_shutdown();
	return 0;
}
