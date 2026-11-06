#!/bin/sh
# Bolum01..Bolum15 altindaki her C programini derler ve calistirir.
# Butun hatalari toplar (ilk hatada durmaz). Hatalar workflow annotation olarak
# basilir; boylece GitHub'a giris yapmadan da okunabilir.
#
#   CC=gcc sh ci/build.sh
#
# Ilk satirinda "CALISTIRMA: yalniz derleme" yazan dosyalar yalnizca derlenir.
set -u
CC="${CC:-gcc}"
CFLAGS="-std=gnu11 -Wall -Wextra -O2 -pthread"
[ "$CC" = "gcc" ] && CFLAGS="$CFLAGS -Werror"
LIBS="-lm -lrt"
summary="${GITHUB_STEP_SUMMARY:-/dev/stdout}"
label="${RUNNER_OS:-local}/$CC"
root=$(pwd)
tmp=$(mktemp -d)
failed=""
ok=0

printf '\n## C programlari — %s\n\n| Dosya | Sonuc |\n|---|---|\n' "$label" >> "$summary"

for f in Bolum*/*.c; do
    [ -f "$f" ] || continue
    name=$(echo "$f" | tr '/.' '__')
    exe="$tmp/$name"
    if ! $CC $CFLAGS "$f" -o "$exe" $LIBS > "$tmp/$name.log" 2>&1; then
        failed="$failed $f"
        printf '| `%s` | **DERLENMEDI** |\n' "$f" >> "$summary"
        continue
    fi
    if head -1 "$f" | grep -q 'CALISTIRMA: yalniz derleme'; then
        ok=$((ok + 1))
        printf '| `%s` | derlendi |\n' "$f" >> "$summary"
        continue
    fi
    run="$tmp/run_$name"
    mkdir -p "$run"
    if (cd "$run" && timeout 20 "$exe" < /dev/null > "$tmp/$name.out" 2>&1); then
        ok=$((ok + 1))
        printf '| `%s` | ok |\n' "$f" >> "$summary"
    else
        rc=$?
        { echo "cikis kodu $rc"; tail -20 "$tmp/$name.out"; } >> "$tmp/$name.log"
        failed="$failed $f"
        printf '| `%s` | **CALISMADI (%s)** |\n' "$f" "$rc" >> "$summary"
    fi
    leftover=$(ls -A "$run")
    if [ -n "$leftover" ]; then
        failed="$failed $f"
        echo "geride kalan dosyalar: $leftover" >> "$tmp/$name.log"
        printf '| `%s` | **DOSYA BIRAKTI** |\n' "$f" >> "$summary"
    fi
done

[ -z "$failed" ] && { printf '\n%s programin hepsi %s uzerinde basarili.\n' "$ok" "$label"; exit 0; }

printf '\n### Hatalar\n' >> "$summary"
for f in $failed; do
    name=$(echo "$f" | tr '/.' '__')
    printf '\n<details><summary>%s</summary>\n\n```\n' "$f" >> "$summary"
    head -40 "$tmp/$name.log" >> "$summary"
    printf '```\n\n</details>\n' >> "$summary"
done

# Hatalari tek annotation'da topla (satir sonlari %0A olarak kodlanir).
msg=$(for f in $failed; do
        name=$(echo "$f" | tr '/.' '__')
        printf '%s:\n' "$f"
        grep -E 'error|warning|cikis kodu|geride kalan|undefined reference' "$tmp/$name.log" \
            | head -4 | sed 's/^/  /'
      done | sed 's/%/%25/g' | awk '{printf "%s%%0A", $0}')
printf '::error title=Bolum programlari %s uzerinde basarisiz::%s\n' "$label" "$msg"

printf '\nBasarisiz:%s\n' "$failed"
cd "$root"
exit 1
