//
// Created by 15857 on 2022/11/28.
//
#include <iostream>
#include <string>
#include <rocksdb/db.h>
extern "C"
{
	#include <git2.h>
	#include <git2/sys/odb_backend.h>
	#include <git2/odb_backend.h>
}
using namespace std;

struct rocks_backend {
	git_odb_backend parent;

	rocksdb::DB* db;
	rocksdb::Options opt;
	rocksdb::Status status;
};

void rocks_backend__free(git_odb_backend *_backend) {
	auto backend = (rocks_backend*)(_backend);
	delete backend->db;
	delete _backend;
}

void getAllkeys(rocksdb::DB* db){
	int cnt = 0;
	rocksdb::Iterator* it = db->NewIterator(rocksdb::ReadOptions());
	for (it->SeekToFirst(); it->Valid(); it->Next()) {
		cnt += 1;
		cout << it->key().ToString() << ": " << it->value().ToString() << endl;
	}
	printf("total key number = %d\n", cnt);
}

int main(){
	rocks_backend *backend{nullptr};
	backend = new rocks_backend();

	backend->opt.create_if_missing = true;

	string path = "./tmp_rocks_2";
	const char *rocksPath = path.c_str();
	printf("%s\n", rocksPath);
	backend->status = rocksdb::DB::Open(backend->opt, rocksPath, &backend->db);
	if(!backend->status.ok()) {
		rocks_backend__free((git_odb_backend *)backend);
		puts("connect to rocksdb faild!");
	}
	else {
		puts("connect to rocksdb success!");
	}

	// rocksdb::WriteBatch batch;
	// batch.Put("k2", "v2");
	// batch.Put("k3", "v3");
	// batch.Put("k4", "v4");
	// rocksdb::Status s = backend->db->Write(rocksdb::WriteOptions(), &batch);

	getAllkeys(backend->db);
	return 0;
}