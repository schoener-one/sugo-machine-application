def BUILD_X86_DEBUG = 'build.x86.debug'
def BUILD_RPI_DEBUG = 'build.rpi.debug'
def BUILD_X86_RELEASE = 'build.x86.release'
def BUILD_RPI_RELEASE = 'build.rpi.release'
def SDK_INSTALL_DIR = 'sdk'

pipeline {
    agent {
        docker {
            image 'testar.jfrog.io/default-docker-local/testar/build:latest'
            registryUrl 'https://testar.jfrog.io'
            registryCredentialsId 'testar-docker-registry'
            reuseNode true
        }
    }
    parameters {
        string(name: 'UPSTREAM_PROJECT_NAME', defaultValue: '/sugo-system/master', description: 'Name of the upstream project')
        string(name: 'BUILD_NUMBER', description: 'Number of the build')
        string(name: 'IMAGE_ARTIFACTS', description: 'Name of the archived image artifact file')
    }
    stages {
        stage('Prepare') {
            when {
                expression { return params.IMAGE_ARTIFACTS.length() > 0 && params.BUILD_NUMBER.length() > 0 }
            }
            stages {
                stage('Fetch SDK') {
                    steps {
                        print "Fetching archived image '${params.IMAGE_ARTIFACTS}'"
                        copyArtifacts fingerprintArtifacts: true,
                            projectName: "${params.UPSTREAM_PROJECT_NAME}",
                            filter: "${params.IMAGE_ARTIFACTS}",
                            selector: specific("${params.BUILD_NUMBER}")
                    }
                } // stage('Fetch SDK')
                stage('Install SDK') {
                    steps {
                        print "Inflate package ${WORKSPACE}/${params.IMAGE_ARTIFACTS}"
                        sh "tar xf '${WORKSPACE}/${params.IMAGE_ARTIFACTS}'"
                        sh "./\$(ls poky-*.sh) -d '${WORKSPACE}/${SDK_INSTALL_DIR}' -y"
                    }
                } // stage('Install SDK')
            }
        } // stage('Prepare')
        stage('Build') {
            // failFast false
            parallel {
                stage('x86-debug') {
                    stages {
                        stage('Init build') {
                            steps {
                                sh "cmake -B ${BUILD_X86_DEBUG} -GNinja -S . -DCMAKE_BUILD_TYPE=Debug -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCODE_COVERAGE=ON"
                            }
                        }
                        stage('Build app') {
                            steps {
                                sh "cmake --build ${BUILD_X86_DEBUG} --target MachineApplication |tee ${BUILD_X86_DEBUG}.log"
                            }
                        }
                        stage('Run cppcheck') {
                            steps {
                                sh "cmake --build ${BUILD_X86_DEBUG} --target cppcheck"
                            }
                        }
                        stage('Build Tests') {
                            steps {
                                sh "cmake --build ${BUILD_X86_DEBUG} --target CommonTest"
                                sh "cmake --build ${BUILD_X86_DEBUG} --target MachineSmokeTest"
                                sh "cmake --build ${BUILD_X86_DEBUG} --target MessageBrokerTest"
                                sh "cmake --build ${BUILD_X86_DEBUG} --target ServiceComponentsTest"
                            }
                        }
                        stage('Run tests') {
                            steps {
                                sh "ctest --test-dir ${BUILD_X86_DEBUG} --output-junit ${WORKSPACE}/${BUILD_X86_DEBUG}.test-result.xml"
                            }
                        }
                    }
                } // stage('x86-debug')
                stage('x86-release') {
                    stages {
                        stage('Init build') {
                            steps {
                                sh "cmake -B ${BUILD_X86_RELEASE} -GNinja -S . -DCMAKE_BUILD_TYPE=Release"
                            }
                        }
                        stage('Build app') {
                            steps {
                                sh "cmake --build ${BUILD_X86_RELEASE} --target MachineApplication |tee ${BUILD_X86_RELEASE}.log"
                            }
                        }
                        stage('Run cppcheck') {
                            steps {
                                sh "cmake --build ${BUILD_X86_RELEASE} --target cppcheck"
                            }
                        }
                        stage('Build Tests') {
                            steps {
                                sh "cmake --build ${BUILD_X86_RELEASE} --target CommonTest"
                                sh "cmake --build ${BUILD_X86_RELEASE} --target MachineSmokeTest"
                                sh "cmake --build ${BUILD_X86_RELEASE} --target MessageBrokerTest"
                                sh "cmake --build ${BUILD_X86_RELEASE} --target ServiceComponentsTest"
                            }
                        }
                        stage('Run tests') {
                            steps {
                                sh "ctest --test-dir ${BUILD_X86_RELEASE} --output-junit ${WORKSPACE}/${BUILD_X86_RELEASE}.test-result.xml"
                            }
                        }
                    }
                } // stage('x86-release')
                stage('rpi-debug') {
                    when {
                        expression { return params.IMAGE_ARTIFACTS.length() > 0 && params.BUILD_NUMBER.length() > 0 }
                    }
                    stages {
                        stage('Init build') {
                            steps {
                                sh """
                                    . \$(ls ${SDK_INSTALL_DIR}/environment-setup-*)
                                    cmake -B ${BUILD_RPI_DEBUG} -GNinja -S . -DCMAKE_BUILD_TYPE=Debug
                                """
                            }
                        } // stage('Init build')
                        stage('Build app') {
                            steps {
                                sh """
                                    . \$(ls ${SDK_INSTALL_DIR}/environment-setup-*)
                                    cmake --build ${BUILD_RPI_DEBUG} --target MachineApplication |tee ${BUILD_RPI_DEBUG}.log
                                """
                            }
                        } // stage('Build app')
                        stage('Build Tests') {
                            steps {
                                sh """
                                    . \$(ls ${SDK_INSTALL_DIR}/environment-setup-*)
                                    cmake --build ${BUILD_RPI_DEBUG} --target CommonTest
                                    cmake --build ${BUILD_RPI_DEBUG} --target MachineSmokeTest
                                    cmake --build ${BUILD_RPI_DEBUG} --target MessageBrokerTest
                                    cmake --build ${BUILD_RPI_DEBUG} --target ServiceComponentsTest
                                """
                            }
                        } // stage('Build Tests')
                        stage('Run tests') {
                            // TODO run in ARM emulator!
                            steps {
                                print "not yet implemented"
                            }
                        }
                    } // stages
                } // tage('rpi-debug')
                stage('rpi-release') {
                    when {
                        expression { return params.IMAGE_ARTIFACTS.length() > 0 && params.BUILD_NUMBER.length() > 0 }
                    }
                    stages {
                        stage('Init build') {
                            steps {
                                sh """
                                    . \$(ls ${SDK_INSTALL_DIR}/environment-setup-*)
                                    cmake -B ${BUILD_RPI_RELEASE} -GNinja -S . -DCMAKE_BUILD_TYPE=Release
                                """
                            }
                        } // stage('Init build')
                        stage('Build app') {
                            steps {
                                sh """
                                    . \$(ls ${SDK_INSTALL_DIR}/environment-setup-*)
                                    cmake --build ${BUILD_RPI_RELEASE} --target MachineApplication |tee ${BUILD_RPI_RELEASE}.log
                                """
                            }
                        } // stage('Build app')
                        stage('Build Tests') {
                            steps {
                                sh """
                                    . \$(ls ${SDK_INSTALL_DIR}/environment-setup-*)
                                    cmake --build ${BUILD_RPI_RELEASE} --target CommonTest
                                    cmake --build ${BUILD_RPI_RELEASE} --target MachineSmokeTest
                                    cmake --build ${BUILD_RPI_RELEASE} --target MessageBrokerTest
                                    cmake --build ${BUILD_RPI_RELEASE} --target ServiceComponentsTest
                                """
                            }
                        } // stage('Build Tests')
                        stage('Run tests') {
                            // TODO run in ARM emulator!
                            steps {
                                print "not yet implemented"
                            }
                        }
                    } // stages
                } // tage('rpi-release')
            } // parallel
        } // stage Run build
        stage('Post checks')
        {
            parallel {
                stage('Check code format') {
                    steps {
                        sh """
                            cmake --build ${BUILD_X86_DEBUG} --target clang_format_check
                            cp ${BUILD_X86_DEBUG}/clang-format.log ${WORKSPACE}/${BUILD_X86_DEBUG}.clang-format.log
                        """
                    }
                }
                stage('Static code analysis') {
                    steps {
                        sh """
                            ./scripts/clang-tidy-check.sh ${BUILD_X86_DEBUG} Common CupRotationTray HardwareAbstractionLayer* Hardware* Machine* ServiceComponents" 2>&1 | \
                            tee clang-tidy-check.log
                        """
                    }
                }
                stage('Check annotations') {
                    steps {
                        createAnnotationLog('Common CupRotationTray HardwareAbstractionLayer* Hardware* Machine* ServiceComponents', 'annotations.log')
                    }
                }
            } // parallel
        } // stage('Post checks')
        stage('Post builds') {
            parallel {
                stage('Build documentation') {
                    steps {
                        sh 'doxygen doxygen.conf 2>&1 |grep \'^!!! \' | sed \'s/^!!! //\'|tee doxygen.warn.log'
                        sh 'tar -zcf sugo-machine.doc.tar.gz sugo-machine.doc'
                    }
                }
                stage('Build coverage report') {
                    steps {
                        sh "cmake --build ${BUILD_X86_DEBUG} --target coverage_xml"
                        sh "mv ${BUILD_X86_DEBUG}/coverage.xml coverage.xml"
                        publishMyCoverage('coverage.xml')
                    }
                }
            } // parallel
        } // stage('Post builds')
    }
    post {
        always {
            // TODO Create one single artifact package per run!
            archiveArtifacts artifacts: '*.test-result.xml', fingerprint: true
            archiveArtifacts artifacts: 'doxygen.warn.log', fingerprint: true
            archiveArtifacts artifacts: 'clang-tidy-check.log', fingerprint: true
            archiveArtifacts artifacts: 'annotations.log', fingerprint: true
            archiveArtifacts artifacts: 'coverage.xml', fingerprint: true
            archiveArtifacts artifacts: 'build.*.log', fingerprint: true
            archiveArtifacts artifacts: 'sugo-machine.doc.tar.gz', fingerprint: true

            // TODO set appropriate filters for removing test files!
            recordIssues enabledForFailure: false, tool: gcc(pattern: 'build.*.log')
            recordIssues enabledForFailure: false, tool: issues(name: 'Annotations', pattern: 'annotations.log')
            recordIssues enabledForFailure: false, tool: clangTidy(pattern: 'clang-tidy-check.log')
            recordIssues enabledForFailure: false, tool: doxygen(pattern: 'doxygen.warn.log')
            recordIssues enabledForFailure: false, tool: junitParser(pattern: '*.test-result.xml')
            junit '*.test-result.xml'
        }
    }
}

def publishBuildIssues(String id) {
    script {
        try {
            def _issues = scanForIssues tool: gcc()
            def _id = id.replaceAll('\\.', '-')
            def _name = id.replaceAll('\\.', ' ')
            publishIssues id: "${_id}", name: "${_name}", issues: [ _issues ]
        }
        catch (Exception e) {
            error("!!! Pipeline error: ${e}")
        }
    }
}

def publishMyCoverage(String path) {
    script {
        try {
            cobertura coberturaReportFile: "${path}"
        }
        catch (Exception e) {
            error("!!! Pipeline error: ${e}")
        }
    }
}

def createAnnotationLog(String searchPattern, String annotationLogFile) {
    sh """
        find ${searchPattern} | \
        grep -E "\\.hpp|\\.cpp" | \
        xargs grep -iEnH "//\\s*TODO|//\\s*FIXME" | \
        sed 's/:\\([0-9]*\\):.*\\/\\/[ ]*\\([A-Z]*\\) /:\\1:\\2:/g' | \
        awk -F: '{end=\$2; print "{ \\"fileName\\": \\""\$1"\\", \\"lineStart\\": "\$2", \\"lineEnd\\": "\$2", \\"severity\\": \\""\$3"\\", \\"message\\": \\""\$3": "\$4"\\" }"}' | \
        sed 's/TODO/NORMAL/' | \
        sed 's/FIXME/HIGH/' | \
        tee ${annotationLogFile}
    """
}
