pipeline {
    agent {
        kubernetes {
            inheritFrom 'rust'
        }
    }

    stages {
        stage('Сборка') {
            steps {
                dir('rust') {
                    sh 'cargo build'
                }
            }
        }
        stage('Cmake tests') {
            steps {
                dir('rust') {
                    sh 'cargo test'
                }
                dir('build') {
                    sh 'cmake -DCMAKE_BUILD_TYPE=Release ..'
                    sh 'cmake --build . --config Release'
                    sh 'ctest'
                }
            }
        }
    }
}
