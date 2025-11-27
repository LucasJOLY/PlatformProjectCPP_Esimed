#!/bin/bash
# Script pour exécuter les fichiers C++ avec Code Runner

# Vérifier qu'un argument a été fourni
if [ -z "$1" ]; then
    echo "❌ Erreur: aucun fichier spécifié"
    exit 1
fi

# Récupérer tous les arguments (pour gérer les chemins avec espaces)
# Code Runner peut passer le chemin avec des guillemets doubles ou simples
FILE_PATH="$*"

# Nettoyer tous les types de guillemets au début et à la fin
# Supprimer les guillemets doubles de manière répétée (cas où Code Runner en ajoute plusieurs)
while [[ "$FILE_PATH" =~ ^\"(.*)\"$ ]] || [[ "$FILE_PATH" =~ ^\"\"(.*)\"\"$ ]]; do
    if [[ "$FILE_PATH" =~ ^\"\"(.*)\"\"$ ]]; then
        FILE_PATH="${BASH_REMATCH[1]}"
    elif [[ "$FILE_PATH" =~ ^\"(.*)\"$ ]]; then
        FILE_PATH="${BASH_REMATCH[1]}"
    else
        break
    fi
done
# Supprimer les guillemets simples
FILE_PATH="${FILE_PATH#\'}"
FILE_PATH="${FILE_PATH%\'}"

# Convertir en chemin absolu si nécessaire
if [[ ! "$FILE_PATH" = /* ]]; then
    FILE_PATH="$(pwd)/$FILE_PATH"
fi

# Vérifier que le fichier existe
if [ ! -f "$FILE_PATH" ]; then
    echo "❌ Erreur: fichier non trouvé: $FILE_PATH"
    exit 1
fi

FILE_DIR=$(dirname "$FILE_PATH")
FILE_NAME=$(basename "$FILE_PATH")

# Trouver le répertoire racine du projet (où se trouve CMakeLists.txt)
PROJECT_ROOT="$FILE_DIR"
while [ ! -f "$PROJECT_ROOT/CMakeLists.txt" ] && [ "$PROJECT_ROOT" != "/" ]; do
    PROJECT_ROOT=$(dirname "$PROJECT_ROOT")
done

# Vérifier que CMakeLists.txt a été trouvé
if [ ! -f "$PROJECT_ROOT/CMakeLists.txt" ]; then
    echo "❌ Erreur: CMakeLists.txt non trouvé dans le répertoire parent"
    exit 1
fi

# Vérifier si le répertoire build est configuré correctement
cd "$PROJECT_ROOT" || exit
if [ ! -f "build/CMakeCache.txt" ] || [ ! -f "build/build.ninja" ]; then
    echo "⚙️  Configuration de CMake..."
    # Nettoyer le répertoire build s'il existe mais n'est pas configuré avec Ninja
    if [ -d "build" ] && [ ! -f "build/build.ninja" ]; then
        echo "🧹 Nettoyage du répertoire build..."
        rm -rf build
    fi
    mkdir -p build
    cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=BOOL=TRUE -DCMAKE_CXX_COMPILER=/opt/homebrew/opt/llvm/bin/clang++ -G Ninja -S . -B build > /dev/null 2>&1
    if [ $? -ne 0 ] || [ ! -f "build/build.ninja" ]; then
        echo "❌ Erreur lors de la configuration CMake"
        exit 1
    fi
fi

# Si le fichier est dans le dossier tests/, exécuter les tests
if [[ "$FILE_DIR" == *"tests"* ]] || [[ "$FILE_PATH" == *"tests"* ]]; then
    echo "🔍 Détection: fichier de test détecté"
    echo "📦 Compilation des tests avec CMake..."
    cmake --build build --target tests
    if [ $? -eq 0 ]; then
        echo ""
        echo "✅ Exécution des tests..."
        echo ""
        ./build/tests
    else
        echo "❌ Erreur lors de la compilation"
        exit 1
    fi
# Sinon, exécuter le programme principal
else
    echo "🔍 Détection: programme principal détecté"
    echo "📦 Compilation avec CMake..."
    cmake --build build --target mon_projet
    if [ $? -eq 0 ]; then
        echo ""
        echo "✅ Exécution du programme..."
        echo ""
        ./build/mon_projet
    else
        echo "❌ Erreur lors de la compilation"
        exit 1
    fi
fi

