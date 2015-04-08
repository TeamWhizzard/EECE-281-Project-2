#!/bin/bash

# downloads IP address from curlmyip.com
ipVar=$(curl curlmyip.com)

# sends PushBullet message of IP address
curl https://api.pushbullet.com/v2/pushes \
-u l1AB4jAjxYJXCWdsoaewTRyHtJzuFhss: \
-d device_iden="ujz7UjnzJw4sjAiVsKnSTs" \
-d type="note" \
-d title="Pi IP address" \
-d body=$ipVar \
-X POST
