#!/usr/bin/perl
use strict;
use warnings;

use Cache::Memcached::Fast;
my $cache = new Cache::Memcached::Fast {
    'servers' => [
        'localhost:11211',
    ],
};

# Get server versions.
my $versions = $cache->server_versions;
while (my ($server, $version) = each %$versions) {
    print $server, "\n";
    print $version, "\n";
}

my $keys = $cache->getAllKeys();
