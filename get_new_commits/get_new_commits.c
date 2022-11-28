//
// Created by 15857 on 2022/11/7.
//
#include "git2.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
//#include <sys/stat.h>
#include <common.h>
int main(int argc, char **argv)
{
	char *l0 = NULL;
	char seps[] = ",\n";
	size_t size = 0;
	const char *cmsg;
	git_libgit2_init();

	git_repository *repo;
	git_remote *remote;
	git_fetch_options fetch_opts = GIT_FETCH_OPTIONS_INIT;
	// chenkx : add proxy in WSL env
	fetch_opts.proxy_opts.type = GIT_PROXY_SPECIFIED;
	fetch_opts.proxy_opts.url = "http://172.25.32.1:7890";

	const git_indexer_progress *stats;
	printf("233 start!\n");
	while(getline(&l0, &size, stdin) >= 0) {

		/*parse input into path, url, new HEAD, old HEAD*/
		char *path = strtok(l0, seps);
		printf("path: %s\n", path);
		char *url = strtok(NULL, seps);
		printf("url: %s\n", url);
		char* new_head = strtok(NULL, seps);
		printf("new head: %s\n", new_head);
		char* old_head = strtok(NULL, seps);
		printf("old head: %s\n", old_head);
		if (strcmp(new_head, old_head) == 0) {
			printf("no update!\n");
			continue;
		}

		/*open the respository and remote*/
		int error = 0;
		error = access(path, F_OK);
		/*if the dir doesn't exist, then create the dir and init the repository, add remote*/
		if(error < 0) {
			printf("can not open repository, create the dir: %s\n", path);
			error = mkdir(path, 0755);
			error = git_repository_init(&repo, path, 0);
			if (error < 0) {
				printf("init repository error!\n");
			}
			error = git_remote_create(&remote, repo, "origin", url);
			if(error < 0) {
				printf("add remote error!\n");
			}
		}
		/*if the dir exist, open the repository, add remote*/
		else {
			error = git_repository_open(&repo, path);
			if(error < 0) {
				error = git_repository_init(&repo, path, 0);
				if (error < 0) {
					printf("init repository error!\n");
				}
				error = git_remote_create(&remote, repo, "origin", url);
				if(error < 0) {
					printf("add remote error!\n");
				}
			}
			else {
				error = git_remote_lookup(&remote, repo, "origin");
				if(error < 0) {
					printf("find remote error!\n");
				}
			}
		}
		//fetch from the remote
		error = git_remote_fetch(remote, NULL, &fetch_opts, NULL);
		if(error < 0) {
			printf("fetch error!\n");
		}
		//const git_transfer_progress *stats;
		stats = git_remote_stats(remote);
		if (stats->local_objects > 0) {
			printf("\rReceived %u/%u objects in %" PRIuZ " bytes (used %u local objects)\n",
			       stats->indexed_objects, stats->total_objects, stats->received_bytes, stats->local_objects);
		} else{
			printf("\rReceived %u/%u objects in %" PRIuZ " bytes\n",
			       stats->indexed_objects, stats->total_objects, stats->received_bytes);
		}
		for(int i=0; i<10; i++)
			printf("\n");
		git_oid head_oid, fetch_head_oid;
		puts("???");
		git_strarray refs = {0};
		char longsha[41] = {0};
		error = git_reference_list(&refs, repo);
		for(int i=0; i<refs.count; i++) {
			printf("%s\t", refs.strings[i]);
			error = git_reference_name_to_id(&head_oid, repo, refs.strings[i]);
			git_oid_tostr(longsha, 41, &head_oid);
			printf("%s\n", longsha);
		}
//		git_strarray fetch_refspecs = {0};
//		int error = git_remote_get_fetch_refspecs(&fetch_refspecs, remote);
		/* Make a shortened printable string from an OID */

		for(int i=0; i<10; i++) {
			printf("\n");
		}
		continue;
		/*
		puts("???");
		error = git_reference_name_to_id(&fetch_head_oid, repo, 'FETCH_HEAD');
		puts("???");
		if(git_oid_equal(&head_oid, &fetch_head_oid)){
			printf("\nalready up date!\n");
		}
		else{
			printf("\nnot already up todate\n");
		}
		puts("???");
		 */
		//get new commits
		git_oid new_oid, old_oid, oid;
		git_oid_fromstr(&new_oid, new_head);
		git_oid_fromstr(&old_oid, old_head);
		git_revwalk *walk;
		git_revwalk_new(&walk, repo);
		git_revwalk_sorting(walk, GIT_SORT_TOPOLOGICAL);
		git_revwalk_push(walk, &new_oid);
		git_revwalk_hide(walk, &old_oid);
		git_commit *wcommit;
		while((git_revwalk_next(&oid, walk)) == 0) {
			error = git_commit_lookup(&wcommit, repo, &oid);
			cmsg  = git_commit_message(wcommit);
			char name[GIT_OID_SHA1_HEXSIZE+1];
			git_oid_tostr(name, GIT_OID_SHA1_HEXSIZE+1, &oid);
			printf("%s: %s\n", name, cmsg);
			git_commit_free(wcommit);
		}
		git_revwalk_free(walk);
		git_repository_free(repo);
		git_remote_free(remote);
	}


	git_libgit2_shutdown ();

	return 0;
}