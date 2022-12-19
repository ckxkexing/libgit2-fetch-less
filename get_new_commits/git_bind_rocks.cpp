//
// Created by 15857 on 2022/12/19.
//
#include<iostream>

extern "C" {
    #include "git2/sys/repository.h"
	#include "git2.h"
}

using namespace std;

int main(){
   	git_libgit2_init();
	const char *url = "https://github.com/lu-zero/cargo-c.git";
	const char *path = "../tmp";
	git_repository *repo = NULL;
    git_remote *remote;

	int error = git_repository_open(&repo, path);
    //
    // /* lookup the remote */
    // error = git_remote_lookup(&remote, repo, "origin");
    // error = git_remote_fetch(remote,
    //                          NULL, /* refspecs, NULL to use the configured ones */
    //                          NULL, /* options, empty for defaults */
    //                          NULL); /* reflog mesage, usually "fetch" or "pull", you can leave it NULL for "fetch" */
    //

    // git_odb_new
    git_odb * db;
    git_odb_new(&db);

    git_odb_backend *rocksdb;
    git_odb_backend_rocks(&rocksdb, "./tmp_rocks_2");

    // git_odb_add_backend
    git_odb_add_backend(db, rocksdb, 1);

    // git_repository_set_odb
    git_repository_set_odb(repo, db);
    int num = git_odb_num_backends(db);
    cout << num << endl;

    // rocksdb 需要支持pack才能直接读取pack的内容。
    // git_odb_write_multi_pack_index(db);

    // use git_odb_write
    git_oid oid;
    git_odb_write(&oid, db, "Test data\n", 10, GIT_OBJECT_BLOB);

    // use git_odb_open_wstream
    // Using write stream is preferred.

    // 好像还有个ref数据库

	git_libgit2_shutdown();
	return 0;
}
