#!/usr/bin/env bash
############################################
# Aufruf mit z.B.: ./enter.sh 
# Vorab xy durch eigenen Usernamen ersetzen
############################################
USERNAME=xy
ROOTFS="rootfs"

set -e


if [ "$EUID" -ne 0 ]; then
    exec sudo "$0" "$@"
fi

# Workingdirectory erstellen, falls noch nicht vorhanden:
[ -d "/home/$USERNAME/Dokumente/modsimdata" ] || {
	echo Workingdirectory /home/$USERNAME/Dokumente/modsimdata wird erstellt
	echo Dieses wird unter rootfs/home/modsim/Dokumente/modsimdata_extern gemountet
	mkdir -p "/home/$USERNAME/Dokumente/modsimdata"
	chown $USERNAME:$USERNAME "/home/$USERNAME/Dokumente/modsimdata"
}


############################################
# Mount
############################################

mount --bind /proc "$ROOTFS/proc"
mount --bind /sys "$ROOTFS/sys"
mount --bind /dev "$ROOTFS/dev"
mount --bind /dev/pts "$ROOTFS/dev/pts"
mount --bind /dev/shm "$ROOTFS/dev/shm"
mount --bind "/home/$USERNAME/Dokumente/modsimdata" "$ROOTFS/home/modsim/Dokumente/modsimdata_extern"

############################################
# Cleanup Handler
############################################

cleanup() {
    echo "Unmounting rootfs..."

    umount -lf "$ROOTFS/dev/pts" 2>/dev/null || true
    umount -lf "$ROOTFS/dev" 2>/dev/null || true
    umount -lf "$ROOTFS/proc" 2>/dev/null || true
    umount -lf "$ROOTFS/sys" 2>/dev/null || true
    umount -lf "$ROOTFS/dev/shm" 2>/dev/null || true
    umount -lf "$ROOTFS/home/modsim/Dokumente/modsimdata_extern" 2>/dev/null || true
}

trap cleanup EXIT

############################################
# Start chroot
############################################

DISPLAY_VAR="${DISPLAY:-:0}"

chroot \
  --userspec=modsim:modsim \
  "$ROOTFS" \
  /usr/bin/env \
    DISPLAY="$DISPLAY_VAR" \
    HOME="/home/modsim" \
    USER="modsim" \
    LOGNAME="modsim" \
    SHELL="/bin/bash" \
    TERM="$TERM" \
    /bin/bash -lc "cd ~ && exec /bin/bash -l"


