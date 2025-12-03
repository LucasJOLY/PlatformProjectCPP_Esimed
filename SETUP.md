# Configuration requise

Ce projet nécessite plusieurs outils et extensions pour fonctionner correctement.

## Prérequis système (macOS)

### Homebrew

Assurez-vous d'avoir [Homebrew](https://brew.sh/) installé sur votre système.

### Outils de build

Installez les outils suivants via Homebrew :

```bash
brew install cmake
brew install ninja
```

### Compilateur C++

Le projet nécessite un compilateur C++23 avec support des modules. Il est recommandé d'utiliser LLVM via Homebrew :

```bash
brew install llvm
```

## Extensions VS Code requises

Pour utiliser ce projet dans Visual Studio Code, les extensions suivantes sont **requises** :

1. **Code Runner** (`formulahendry.code-runner`)

   - Permet d'exécuter directement les fichiers C++ depuis l'éditeur
   - Configuration automatique incluse dans `.vscode/settings.json`

2. **C/C++** (`ms-vscode.cpptools`)
   - Fournit l'intelligence artificielle pour le C++
   - Améliore l'autocomplétion et la navigation dans le code

### Installation des extensions

Vous pouvez installer ces extensions de deux façons :

**Via l'interface VS Code :**

1. Ouvrez VS Code
2. Allez dans l'onglet Extensions (⌘+Shift+X)
3. Recherchez "Code Runner" et installez-le
4. Recherchez "C/C++" et installez-le

## Vérification de l'installation

Pour vérifier que tout est correctement installé :

```bash
# Vérifier CMake
cmake --version

# Vérifier Ninja
ninja --version

# Vérifier le compilateur LLVM
/opt/homebrew/opt/llvm/bin/clang++ --version
```

## Configuration du projet

Une fois tous les prérequis installés :

1. Clonez ou téléchargez ce projet
2. Ouvrez le dossier dans VS Code
3. Les extensions seront automatiquement détectées
4. Utilisez le bouton "Run Code" (▶️) en haut à droite de l'éditeur pour exécuter vos fichiers C++

## Utilisation

- **Fichiers dans `src/`** : Exécutent le programme principal (`PlatformProjectCPP_Esimed`)
- **Fichiers dans `tests/`** : Compilent et exécutent les tests unitaires

Le script `.vscode/run_cpp.sh` détecte automatiquement le type de fichier et exécute la bonne commande CMake.
