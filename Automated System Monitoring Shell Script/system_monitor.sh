#!/bin/bash
#
# Automated System Monitoring Shell Script
# Monitors CPU, memory, disk usage, and top processes.
# Provides thresholds, logging, and an interactive menu interface.

LOG_FILE="$HOME/system_monitor.log"
INTERVAL=60  # default monitoring interval in seconds

# Default thresholds (in %)
CPU_THRESHOLD=80
MEM_THRESHOLD=80
DISK_THRESHOLD=80

########################
# Utility / Setup      #
########################

check_dependencies() {
    local missing=0
    for cmd in top free df ps awk date; do
        if ! command -v "$cmd" >/dev/null 2>&1; then
            echo "Error: Required command '$cmd' not found in PATH."
            missing=1
        fi
    done

    if [[ $missing -eq 1 ]]; then
        echo "Please install the missing commands and rerun the script."
        exit 1
    fi
}

log_message() {
    local msg="$1"
    local ts
    ts="$(date '+%Y-%m-%d %H:%M:%S')"
    echo "[$ts] $msg" | tee -a "$LOG_FILE"
}

init_log() {
    if ! touch "$LOG_FILE" 2>/dev/null; then
        echo "Error: Cannot write to log file '$LOG_FILE'. Check permissions."
        exit 1
    fi
}

########################
# Metrics Collectors   #
########################

get_cpu_usage() {
    # Parse CPU usage from top (non-interactive)
    # Assumes a "Cpu(s)" line with idle percentage.
    local cpu
    cpu=$(top -bn1 | awk -F',' '/Cpu\(s\)/ {
        idle=0
        for (i=1;i<=NF;i++) {
            if ($i ~ /id/) {
                gsub(".* ", "", $i)
                gsub("%", "", $i)
                gsub("id", "", $i)
                idle=$i
            }
        }
        if (idle == 0) idle=0
        printf "%.0f", 100-idle
    }')

    # Fallback if parsing fails
    if [[ -z "$cpu" ]]; then
        cpu=0
    fi
    echo "$cpu"
}

get_mem_usage() {
    # Uses 'free' to calculate memory usage %
    free | awk '/Mem:/ {printf "%.0f", $3/$2*100}'
}

get_disk_usage() {
    # Checks root filesystem usage %
    df -P / | awk 'NR==2 {gsub("%","",$5); print $5}'
}

show_top_processes() {
    # Top 5 CPU-consuming processes
    ps -eo pid,comm,%cpu,%mem --sort=-%cpu | head -n 6
}

########################
# Monitoring Logic     #
########################

check_thresholds_and_alert() {
    local cpu=$1
    local mem=$2
    local disk=$3

    if (( cpu > CPU_THRESHOLD )); then
        log_message "WARNING: CPU usage high (${cpu}% > ${CPU_THRESHOLD}%)"
    fi

    if (( mem > MEM_THRESHOLD )); then
        log_message "WARNING: Memory usage high (${mem}% > ${MEM_THRESHOLD}%)"
    fi

    if (( disk > DISK_THRESHOLD )); then
        log_message "WARNING: Disk usage high (${disk}% > ${DISK_THRESHOLD}%)"
    fi
}

view_system_status() {
    local cpu mem disk

    cpu=$(get_cpu_usage)
    mem=$(get_mem_usage)
    disk=$(get_disk_usage)

    log_message "===== System Status ====="
    log_message "CPU Usage   : ${cpu}%"
    log_message "Memory Usage: ${mem}%"
    log_message "Disk Usage  : ${disk}% (root filesystem)"

    echo "Top 5 processes by CPU:" | tee -a "$LOG_FILE"
    show_top_processes | tee -a "$LOG_FILE"

    check_thresholds_and_alert "$cpu" "$mem" "$disk"
}

continuous_monitoring() {
    echo "Starting continuous monitoring every ${INTERVAL} seconds."
    echo "Press Ctrl+C to stop."

    while true; do
        view_system_status
        sleep "$INTERVAL"
    done
}

########################
# Menu Actions         #
########################

set_thresholds() {
    echo "Current thresholds:"
    echo "  CPU : $CPU_THRESHOLD%"
    echo "  MEM : $MEM_THRESHOLD%"
    echo "  DISK: $DISK_THRESHOLD%"
    echo

    read -rp "Enter new CPU threshold (0-100, blank to keep current): " cpu
    if [[ -n "$cpu" ]]; then
        if [[ "$cpu" =~ ^[0-9]+$ ]] && (( cpu >= 0 && cpu <= 100 )); then
            CPU_THRESHOLD=$cpu
        else
            echo "Invalid CPU threshold. Keeping old value."
        fi
    fi

    read -rp "Enter new Memory threshold (0-100, blank to keep current): " mem
    if [[ -n "$mem" ]]; then
        if [[ "$mem" =~ ^[0-9]+$ ]] && (( mem >= 0 && mem <= 100 )); then
            MEM_THRESHOLD=$mem
        else
            echo "Invalid Memory threshold. Keeping old value."
        fi
    fi

    read -rp "Enter new Disk threshold (0-100, blank to keep current): " disk
    if [[ -n "$disk" ]]; then
        if [[ "$disk" =~ ^[0-9]+$ ]] && (( disk >= 0 && disk <= 100 )); then
            DISK_THRESHOLD=$disk
        else
            echo "Invalid Disk threshold. Keeping old value."
        fi
    fi

    log_message "Thresholds updated: CPU=${CPU_THRESHOLD}%, MEM=${MEM_THRESHOLD}%, DISK=${DISK_THRESHOLD}%"
}

view_logs() {
    if [[ ! -s "$LOG_FILE" ]]; then
        echo "Log file is empty or does not exist."
        return
    fi

    echo "===== LOG FILE: $LOG_FILE ====="
    less "$LOG_FILE"
}

clear_logs() {
    > "$LOG_FILE"
    echo "Log file cleared."
}

set_interval() {
    read -rp "Enter monitoring interval in seconds (current: $INTERVAL): " new_interval
    if [[ "$new_interval" =~ ^[0-9]+$ ]] && (( new_interval > 0 )); then
        INTERVAL=$new_interval
        echo "Monitoring interval set to ${INTERVAL} seconds."
    else
        echo "Invalid interval. Keeping previous value: ${INTERVAL} seconds."
    fi
}

########################
# Menu Loop            #
########################

show_menu() {
    echo
    echo "===== System Monitoring Menu ====="
    echo "1) View system status (single run)"
    echo "2) Set alert thresholds"
    echo "3) View logs"
    echo "4) Clear logs"
    echo "5) Set monitoring interval (seconds)"
    echo "6) Start continuous monitoring"
    echo "7) Exit"
    echo "=================================="
}

main() {
    check_dependencies
    init_log

    while true; do
        show_menu
        read -rp "Enter choice [1-7]: " choice

        case "$choice" in
            1)
                view_system_status
                ;;
            2)
                set_thresholds
                ;;
            3)
                view_logs
                ;;
            4)
                clear_logs
                ;;
            5)
                set_interval
                ;;
            6)
                continuous_monitoring
                ;;
            7)
                echo "Exiting. Goodbye!"
                exit 0
                ;;
            *)
                echo "Invalid choice. Please enter a number between 1 and 7."
                ;;
        esac
    done
}

main