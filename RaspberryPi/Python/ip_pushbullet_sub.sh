#!/bin/bash
#ipVar=$(/sbin/ifconfig $(netstat -nr | tail -1 | awk '{print $NF}') | awk -F: '/inet /{print $2}' | cut -f1 -d ' ')
ipVar=$(curl curlmyip.com)
curl https://api.pushbullet.com/v2/pushes \
-u l1AB4jAjxYJXCWdsoaewTRyHtJzuFhss: \
-d device_iden="ujz7UjnzJw4sjAiVsKnSTs" \
-d type="note" \
-d title="Pi IP address" \
-d body=$ipVar \
-X POST
