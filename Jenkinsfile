pipeline {
    agent {
        kubernetes {
            cloud 'kubernetes'
            containerTemplate(
                name: 'rust',
                image: 'rust:1.74.1',
                command: 'cat',
                ttyEnabled: true,
                resourceRequestCpu: '1',
                resourceLimitCpu: '4',
                resourceRequestMemory: '512Mi',
                resourceLimitMemory: '2Gi',
            )
            defaultContainer 'rust'
            inheritFrom 'default'
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
