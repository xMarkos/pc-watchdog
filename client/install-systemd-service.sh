#!/bin/bash

declare SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

declare service_file=/etc/systemd/system/watchdog-pulser.service

echo "Initializing python virtual environment"

if ! (cd "$SCRIPT_DIR" && python -m venv env && env/bin/pip install -r requirements.txt); then
	>&2 echo "Could not initialize python virtual environment"
	exit 1
fi

echo "Installing systemd unit file"

cat > "$service_file" <<EOF
[Unit]
Description=Watchdog Pulser Service
After=network.target

# Allow 50 attempts within 10 minutes
StartLimitIntervalSec=600
StartLimitBurst=50

[Service]
Type=exec
WorkingDirectory=$SCRIPT_DIR
ExecStart=$SCRIPT_DIR/env/bin/python main.py
Restart=always
User=root
Group=root
TimeoutStopSec=5
KillSignal=SIGINT
StandardOutput=append:/var/log/watchdog.log
StandardError=journal

# Wait 5 seconds before attempting a restart
RestartSec=5

[Install]
WantedBy=multi-user.target
EOF

systemctl daemon-reload
systemctl enable "$service_file"

echo "Done. To start, use systemd start watchdog-pulser"
