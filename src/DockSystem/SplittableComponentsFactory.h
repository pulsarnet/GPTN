//
// Created by nmuravev on 3/21/2022.
//

#ifndef FFI_RUST_SPLITTABLECOMPONENTSFACTORY_H
#define FFI_RUST_SPLITTABLECOMPONENTSFACTORY_H

#include <DockComponentsFactory.h>
#include <DockAreaTitleBar.h>

class SplittableComponentsFactory : public ads::CDockComponentsFactory
{
public:
    ads::CDockAreaTitleBar* createDockAreaTitleBar(ads::CDockAreaWidget* dockArea) const override;
};


#endif //FFI_RUST_SPLITTABLECOMPONENTSFACTORY_H
